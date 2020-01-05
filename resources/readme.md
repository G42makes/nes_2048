Using NES Screen Tool 2.4(in Wine on Mac actually, works great). https://shiru.untergrund.net/software.shtml#thumb

`cat test.rle | hexdump -v -e '" \n .byte  " 32/1 "$%02x,"' | cut -c 2-135`

is the command that is useful, modify line length by changing the 32 and 135 values

