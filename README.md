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

## Usage

The `pipe` function should now be registered in your SQLite session. The
signature looks like this:

```
pipe(TEXT cmd [, stdin]);
```

The first parameter should be a string and will be evaluated with `/bin/sh -c`.
The second parameter which is optional will be piped to `sh` through `STDIN`.
The `STDOUT` from `sh` will then be captured and returned from `pipe`.

## Examples

Hello world! Meow~!

```sql
SELECT pipe('cat', 'Hello world!');
```

Use power math!

```sql
SELECT pipe('bc', '2^12' || x'0A');
```

Insert files as blobs:

```sql
CREATE TABLE files (id INTEGER PRIMARY KEY AUTOINCREMENT, data BLOB);
INSERT INTO files (data) VALUES (pipe('cat ./file.bin'));
```

Export blobs as files:

```sql
SELECT pipe('cat > ./file-' || id, data) FROM files;
```

Create a table with MD5 checksums from file blobs:

```sql
CREATE TABLE checksums (id INT, md5 TEXT);
INSERT INTO checksums SELECT id, pipe('md5', data) FROM files;
```

Hash plain text passwords using OpenSSL:

```sql
UPDATE users SET pw=pipe('openssl passwd -1 -stdin', pw);
```

Compress PNG images in MBTiles to JPEG using ImageMagick:

```sql
UPDATE images SET tile_data=pipe('convert -format jpg -quality 90% png:- jpg:-', tile_data);
VACUUM images;
```


```
      |\      _,,,---,,_
ZZZzz /,`.-'`'    -.  ;-;;,_
     |,4-  ) )-,_. ,\ (  `'-'
    '---''(_/--'  `-'\_)
```
