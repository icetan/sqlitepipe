# SQLite Pipe

*Pipe BLOB and TEXT values thru shell commands*

Inspired by defunct
[MBPipe](https://github.com/mapbox/node-mbtiles/wiki/Post-processing-MBTiles-with-MBPipe).

## Prerequisites

SQLite 3 with dynamic extension loading enabled.

On OS X you can install `sqlite3` with `brew` but don't forget that it won't
override the default `sqlite3` command. You need to run it from with
`$(brew --prefix sqlite3)/bin/sqlite3` or add `$(brew --prefix sqlite3)/bin` to
`PATH`.

## Installation

To compile the C code:

```sh
./build
```

This will create a `pipe.sqlext` binary which will be loadable from SQLite.

## Usage

Load the SQLite extension:

```sql
SELECT load_extension('pipe.sqlext');
```

Try it out:

```sql
SELECT pipe('echo Hello, world!');
```

Pipe to stdin and do some power maths:

```sql
SELECT pipe('bc', '2^32');
```


## Examples

Compress PNG images in MBTiles to JPEG using ImageMagick:

```sql
UPDATE images SET tile_data=pipe('convert -format jpg -quality 90% png:- jpg:-', tile_data);
VACUUM images;
```

Hash plain text passwords using OpenSSL:

```sh
UPDATE users SET pw=pipe('openssl passwd -1 -stdin', pw);
```

Insert files as blobs:

```sql
CREATE TABLE files (id INTEGER PRIMARY KEY AUTOINCREMENT, data BLOB);
INSERT INTO files (data) VALUES (pipe('cat file.bin'));
```

Create a table with MD5 checksums from file blobs:

```sql
CREATE TABLE checksums (id INT, md5 TEXT);
INSERT INTO checksums SELECT id, pipe('md5', data) FROM files;
```

```
      |\      _,,,---,,_
ZZZzz /,`.-'`'    -.  ;-;;,_
     |,4-  ) )-,_. ,\ (  `'-'
    '---''(_/--'  `-'\_)
```
