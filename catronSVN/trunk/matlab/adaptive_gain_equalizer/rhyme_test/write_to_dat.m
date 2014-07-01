function [] = write_to_dat(x, data_directory, file_name)
%WRITE_TO_DAT - Store data in file woth format .dat
%
% Syntax:  [] = write_to_dat(x, data_directory, file_name)
%
% Inputs:
%    x - Data in the interval [-32768, 32767]
%    data_directory - The directory where output file should be saved
%    file_name - Name of output file
%
% Outputs:
%    None
%
% Example: 
%    x = [0:32767];
%    write_to_dat(x, pwd, 'x.dat')
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none

 DIR_HOME=pwd;
% if data_directory ~= ''
 %	cd(data_directory);
 %end

n=length(x);
x=round(x); %Make integers

fid=fopen(file_name,'w');

fprintf('Saving dat-file ...\n');
fprintf('%d samples\n',n);
for(i=1:n)
    if(x(i)>32767 || x(i)<-32768)
        fprintf('WARNING:\twrite_to_dat.m\tInput out of range\n');
    end
end

format

for(i=1:n-1)
    temp=min(x(i),32767); %Prevent overflow
    temp=max(temp,-32768); %Prevent overflow
    fprintf(fid,'%d\r\n',temp);
end
temp=min(x(n),32767); %Prevent overflow
temp=max(temp,-32768); %Prevent overflow
fprintf(fid,'%d\r\n',temp);

fclose(fid);
cd(DIR_HOME);
fprintf(' done.\r\n');