function [] = write_float_to_inc(x, data_directory, file_name)
%WRITE_FLOAT_TO_INC - Store data in file with format .inc
%
% Syntax:  [] = write_float_to_inc(x, data_directory, file_name)
%
% Inputs:
%    x - Data in the interval [-1, 1]
%    data_directory - The directory where output file should be saved
%    file_name - Name of output file
%
% Outputs:
%    None
%
% Example: 
%    x = [0:0.001:1];
%    write_float_to_inc(x, pwd, 'x.inc')
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none

% Author - Markus Borgh
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2010-10-25
% Last modification - 2010-10-25

DIR_HOME=pwd;
cd(data_directory);

% Maximum and minimum range
Rmax = 1; % 32767
Rmin = -1; % -32768

n=length(x);

% Open output file
fid=fopen(file_name,'w');

% Prompt user info
fprintf('Saving include-file ...\n');
fprintf('%d samples\n',n);

% Check input data domain
for(i=1:n)
    if(x(i)>Rmax || x(i)<Rmin)
        fprintf('WARNING:\twrite_float_to_inc.m\tInput out of range\n');
    end
end

format long

% Write to file
for(i=1:n-1)
    temp=min(x(i),Rmax); %Prevent overflow
    temp=max(temp,Rmin); %Prevent overflow
    fprintf(fid,'%f,\r\n',temp);
end
% Write last value
temp=min(x(n),Rmax); %Prevent overflow
temp=max(temp,Rmin); %Prevent overflow
fprintf(fid,'%f\r\n',temp);

% Close file and return to home directory
fclose(fid);
cd(DIR_HOME);
fprintf(' done.\r\n');
