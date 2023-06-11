# Advanced Programming
## Assignment 2 - Guy Gur-Arieh
\
To compile the program, type:
```ruby
make stree
```

\
To run the program, type ```./stree```, then a path (optional - default is curret directory).
```ruby
./stree
./stree [PATH]
```

## Example
```ruby
./stree typecheck
```
output:
```ruby
[drwxr-xr-x root     root            4096]  typecheck
├── [crw-r--r-- root     root               0]  chardev
├── [srwxr-xr-x root     root               0]  socket.sock
├── [lrwxrwxrwx root     root               4]  link -> file
├── [-rw-r--r-- root     root               0]  file
├── [prw-r--r-- root     root               0]  fifo
├── [drwxr-xr-x root     root            4096]  dir
└── [brw-r--r-- root     root               0]  blockdev

1 directory, 6 files
```
checks the different file types.