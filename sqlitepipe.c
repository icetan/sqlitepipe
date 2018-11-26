#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT1


// since pipes are unidirectional, we need two pipes. one for data to flow from
// parent's stdout to child's stdin and the other for child's stdout to flow to
// parent's stdin
#define NUM_PIPES          2

#define PARENT_WRITE_PIPE  0
#define PARENT_READ_PIPE   1

// Always in a pipe[], pipe[0] is for read and pipe[1] is for write
#define READ_FD  0
#define WRITE_FD 1

#define PARENT_READ_FD  ( pipes[PARENT_READ_PIPE][READ_FD]   )
#define PARENT_WRITE_FD ( pipes[PARENT_WRITE_PIPE][WRITE_FD] )

#define CHILD_READ_FD   ( pipes[PARENT_WRITE_PIPE][READ_FD]  )
#define CHILD_WRITE_FD  ( pipes[PARENT_READ_PIPE][WRITE_FD]  )

#define BUF_SIZE (4096)

static ssize_t read_fd(char **out, int fd) {
  size_t len = 0;
  size_t size = BUF_SIZE;
  char buf[BUF_SIZE];
  int count;

  char *tmp = malloc(sizeof(char)*size); //size is start size
  if (!tmp) return -1;

  while ((count = read(fd, buf, sizeof(buf)-1)) > 0) {
    if(len + count > size){
      tmp = realloc(tmp, sizeof(char)*(size*=2));
      if (!tmp) return -1;
    }
    memcpy(tmp+(sizeof(char)*len), buf, count);
    len += count;
  }
  if (count < 0) {
    perror("IO Error\n");
    free(tmp);
    return -1;
  }

  *out = realloc(tmp, sizeof(char)*len);
  return len;
}

static ssize_t execCmd(char **out, char **argv, char *data, size_t size) {
  int pipes[NUM_PIPES][2];
  pid_t pid;
  ssize_t outSize;

  // Pipes for parent to write and read
  pipe(pipes[PARENT_READ_PIPE]);
  pipe(pipes[PARENT_WRITE_PIPE]);

  if((pid = fork()) == -1) {
    perror("fork");
    return -1;
  } else if(pid == 0) { // Child process
    dup2(CHILD_READ_FD, STDIN_FILENO);
    dup2(CHILD_WRITE_FD, STDOUT_FILENO);

    // Close fds not required by child. Also, we don't want the exec'ed program
    // to know these existed
    close(CHILD_READ_FD);
    close(CHILD_WRITE_FD);
    close(PARENT_READ_FD);
    close(PARENT_WRITE_FD);

    execv(argv[0], argv);
    // Shouldn't get here
    perror("execv");
    exit(1);
  } else { // Parent process
    // Close fd's, not required by parent
    close(CHILD_READ_FD);
    close(CHILD_WRITE_FD);

    // Write to child’s stdin
    write(PARENT_WRITE_FD, data, size);
    close(PARENT_WRITE_FD);

    // Read from child's stdout
    outSize = read_fd(out, PARENT_READ_FD);
    close(PARENT_READ_FD);

    // Wait for child to exit
    int status;
    waitpid(pid, &status, 0);
    if (status != 0) {
      perror("child exit code not 0");
      return -1;
    }

    return outSize;
  }
}

static void pipeFunc(
  sqlite3_context *context,
  int argc,
  sqlite3_value **argv
) {
  if (argc < 2) {
    sqlite3_result_error(context, "Too few arguments to pipe(), two or more required", -1);
    return;
  }

  char *data = (char*)sqlite3_value_blob(argv[0]);
  size_t dataSize = sqlite3_value_bytes(argv[0]);

  char *cmdArgs[argc];
  for (int i = 0; i < argc-1; i++) {
    cmdArgs[i] = (char*)sqlite3_value_text(argv[i+1]);
  }
  cmdArgs[argc-1] = 0;

  char *done;
  ssize_t doneSize = execCmd(&done, cmdArgs, data, dataSize);

  if (doneSize >= 0) {
    sqlite3_result_blob(context, done, doneSize, SQLITE_TRANSIENT);
    free(done);
  } else {
    sqlite3_result_error(context, "Executing command", -1);
  }
}

// SQLite invokes this routine once when it loads the extension. Create new
// functions, collating sequences, and virtual table modules here.  This is
// usually the only exported symbol in the shared library.
int sqlite3_extension_init(
  sqlite3 *db,
  char **pzErrMsg,
  const sqlite3_api_routines *pApi
) {
  SQLITE_EXTENSION_INIT2(pApi)
  sqlite3_create_function(db, "pipe", -1, SQLITE_UTF8, 0, pipeFunc, 0, 0);
  return 0;
}
