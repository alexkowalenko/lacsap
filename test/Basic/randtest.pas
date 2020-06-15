program randtest;

var
   i : integer;
   r : real;


begin
   for i := 1 to 10 do
   begin
      r := random;
      if (r >= 0.0) and (r < 1.0) then
         writeln(i:1, " TRUE");
   end;
end.
