function convs(infilename, outfilename, offset)
%CONVS - Converts .wav-file to output file with one value per line, i.e. in
% .inc format. Output is written in 16-bit integer format
%
% Syntax:  convs(infilename, outfilename, offset)
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
%    convs('sine1kHz.wav', 'sine1kHz.inc', 0)
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none

% Author - Christian Schüldt
% E-mail - christian.schuldt@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 200X-XX-XX
% Last modification - 2010-10-22

%in = infilename;
in = wavread(infilename);
%in = load(infilename);
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
    fprintf(fid,'%d,\r\n',temp);
end
temp=round(32767*in(n))-offset; %Markus fix
temp=min(temp,32767);
temp=max(temp,-32768);
fprintf(fid,'%d\r\n',temp);

fprintf(' done.\r\n');
fclose(fid);
