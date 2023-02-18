unit other;

interface
var ch : char;

function readkey : char;

implementation

function readkey : char; assembler;
asm
 xor ax, ax
 int 16h
end;

end.