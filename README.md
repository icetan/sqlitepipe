# SQLite Pipe

Pipe stuff through shell commands in SQLite!

Inspired by defunct
[MBPipe](https://github.com/mapbox/node-mbtiles/wiki/Post-processing-MBTiles-with-MBPipe).

## Prerequisites

SQLite 3 with dynamic extension loading enabled.

On OS X you can install `sqlite3` with `brew` to get support for extensions,
but don't forget that it won't override the default `sqlite3` command. You need
to run it with `$(brew --prefix sqlite3)/bin/sqlite3` or add `$(brew --prefix
sqlite3)/bin` to your `PATH`.

## Installation

To download and compile the C code:

```sh
git clone https://github.com/icetan/sqlitepipe
cd sqlitepipe
make
```

This will create the `pipe.sqlext` binary which will be loadable from SQLite.

## Usage

Load the SQLite extension:

```sql
SELECT load_extension('pipe.sqlext');
```

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

```sh
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
