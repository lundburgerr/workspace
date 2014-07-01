function writeToH(x, fileName, variableName, typeName)
% Write to h-file
% Out put will be
% /* fileName */
% 
% #ifndef FILENAME
% #define FILENAME
% 
% #define VARIABLENAME_LENGTH xxxx
% 
% #pragma  DATA_SECTION(variableName, "extvar")
% #pragma  DATA_ALIGN(variableName, 64)
% type variableName[VARIABLENAME_LENGTH] = {
%   values
% };
% 
% #endif FILENAME /* fileName */

fid = fopen(fileName,'wt');
defFile = upper(strrep(fileName, '.', '_'));
defLength = strcat(upper(variableName), '_LENGTH');

fprintf(fid,'/* %s */\n\n', fileName); 

fprintf(fid,'#ifndef %s\n', defFile); 
fprintf(fid,'#define %s\n\n', defFile); 

fprintf(fid,'#define %s %g\n\n', defLength, length(x)); 

fprintf(fid, '#pragma  DATA_SECTION (%s, "extvar")\n', variableName);
fprintf(fid, '#pragma  DATA_ALIGN(%s, 64)\n', variableName);
fprintf(fid,'%s %s[%s] = {\n', typeName, variableName, defLength); 

for i = 1:length(x)    
    fprintf(fid,'%g,\n',x(i)); 
end

fprintf(fid,'};\n\n'); 

fprintf(fid,'#endif /*%s, %s */', defFile, fileName); 

fclose(fid);
