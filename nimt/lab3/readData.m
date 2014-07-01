fid = fopen('first.lvm','rt');
str = fread(fid,'*char')';
% now replace , with .
str = strrep(str,',','.');
% use strread to parse the string
 [a,b,c] = strread(str);