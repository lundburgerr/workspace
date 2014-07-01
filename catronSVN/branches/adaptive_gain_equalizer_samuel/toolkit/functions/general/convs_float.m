function convs_float(infilename, outfilename, offset)
%CONVS_FLOAT - Converts .wav-file to output file with one value per line,
% i.e. in .inc format. Output is written in float
%
% Syntax:  convs_float(infilename, outfilename, offset)
%
% Inputs:
%    infilename - Name of .wav-file
%    outfilename - Name of output file
%    offset - An offset could be used for all values 
%
% Outputs:
%    None
%
% Example: 
%    convs_float('sine1kHz.wav', 'sine1kHz_float.inc', 0)
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none

% Author - Markus Borgh
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2010-03-18
% Last modification - 2010-10-22

in = wavread(infilename);
fid = fopen(outfilename,'w');

offset=round(offset);

n = length(in);
fprintf('%d samples\r\n', n);
fprintf('Saving include-file ...');

in(1);
for i=1:n-1
    temp=round(32767*in(i))-offset; %Markus fix
    temp=min(temp,32767); %Prevent overflow
    temp=max(temp,-32768); %Prevent overflow
    % Convert to float
    temp = temp./32768;
    % Write float
    fprintf(fid,'%f,\r\n',temp);
    % fprintf(fid,'%d,\r\n',temp);
end
temp=round(32767*in(n))-offset; %Markus fix
temp=min(temp,32767);
temp=max(temp,-32768);
% Convert to float
temp = temp./32768;
% Write float
fprintf(fid,'%f\r\n',temp);
%fprintf(fid,'%d\r\n',temp);

fprintf(' done.\r\n');
fclose(fid);
