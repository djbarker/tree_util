Tree Util
=========
*tree_util*

A very simple application to quickly list the directory structure and file counts under linux/unix.

Uses the direct kernel call "getdents" to quickly list the directory structure and count how many files are in each directory. It can handle folders containing upwards of hundreds of thousands of files. 

Unlike the utility 'tree' tree_util does not list regular files it just lists the directory structure and the file counts. It has one optional command line argument which is the folder to list. If called without arguments it lists the working directory.

Sample output:

```
./ [2 files, 3 dirs]
├── src/ [3 files, 0 dirs]
├── release/ [5 files, 1 dirs]
│   └── blah/ [3 files, 2 dirs]
│       ├── snafu/ [0 files, 0 dirs]
│       └── woop/ [5 files, 0 dirs]
└── z_dir/ [0 files, 0 dirs]
```

Note: requires some c++11 features.
