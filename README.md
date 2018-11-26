# SQLite Pipe

Pipe stuff through shell commands in SQLite!

Inspired by defunct
[MBPipe](https://github.com/mapbox/node-mbtiles/wiki/Post-processing-MBTiles-with-MBPipe).

## Prerequisites

SQLite 3 with dynamic extension loading enabled.

### OS X

```sh
brew install sqlite3
```

Don't forget that `brew` won't override the default OS X supplied `sqlite3`. You need
to run it with `$(brew --prefix sqlite3)/bin/sqlite3` or add `$(brew --prefix
sqlite3)/bin` to your `PATH`.

### Ubuntu / Debian

```sh
apt-get install sqlite3 libsqlite3-dev
```

## Installation

To download and compile the C code:

```sh
git clone https://github.com/icetan/sqlitepipe
cd sqlitepipe
make && make install
```

This will create the extension binary and add a load command to your
`~/.sqliterc` which will auto load `sqlitepipe` every time you run `sqlite`.

You can skip `make install` and just run `.load sqlitepipe` in the `sqlite`
REPL to only load it for that session.

## Usage

The `pipe` function should now be registered in your SQLite session. The
signature looks like this:

```
BLOB pipe(NULL | BLOB stdin, TEXT cmd [, TEXT argN..]);
```

The data from the first argument will be piped to `STDIN` of the program defined
in the second argument. The first argument can also be set to `NULL` for no
`STDIN`. The second argument defines what program should be executed and each
following argument will be set as arguments to that program on execution.
The `STDOUT` from the program will then be captured and returned from `pipe` as
`BLOB`.

## Examples

Hello world! Meow~!

```sql
SELECT pipe('Hello world!', '/usr/bin/cat');
```

Use power math!

```sql
SELECT pipe('2^12' || x'0A', '/usr/bin/bc');
```

Insert files as blobs:

```sql
CREATE TABLE files (id INTEGER PRIMARY KEY AUTOINCREMENT, data BLOB);
INSERT INTO files (data) VALUES (pipe(NULL, '/usr/bin/cat', './file.bin'));
```

Export blobs as files:

```sql
SELECT pipe(data, '/usr/bin/cp', '/dev/stdin', './file-' || id) FROM files;
```

Create a table with MD5 checksums from file blobs:

```sql
CREATE TABLE checksums (id INT, md5 TEXT);
INSERT INTO checksums SELECT id, pipe(data, '/usr/bin/md5sum') FROM files;
```

Shell out to `bash`:

```sql
SELECT pipe(data, '/usr/bin/bash', '-c', 'grep "^1234" | sort | uniq | wc') FROM files;
```

Hash plain text passwords using OpenSSL:

```sql
UPDATE users SET pw=pipe(pw, '/usr/bin/openssl', 'passwd', '-1', '-stdin');
```

Compress PNG images in MBTiles to JPEG using ImageMagick:

```sql
UPDATE images SET tile_data=pipe(tile_data, '/usr/bin/convert', '-format', 'jpg', '-quality', '90%', 'png:-', 'jpg:-');
VACUUM images;
```


```
      |\      _,,,---,,_
ZZZzz /,`.-'`'    -.  ;-;;,_
     |,4-  ) )-,_. ,\ (  `'-'
    '---''(_/--'  `-'\_)
```
