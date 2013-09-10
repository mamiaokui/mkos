filesize=$(stat -c '%s' BootProgramLink.o)

#1048576 is 0x100000
if [ $((filesize)) -ge 1048576 ]; then
    echo "error, BootProgramLink.o size is larger than 0x100000"
    rm -f OS.img
fi