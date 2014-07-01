function [] = write_header_file_from_inc(data_name, data_format, data_length_name, header_name, n, filename)
%WRITE_HEADER_FILE_FROM_INC - Reads data values from .inc and writes them
%to a .h-file. The header file contains an #ifndef statement to prevent
%multiple inclusions of the file. The name and format of the data array are
%given as inputs.
%
% Syntax:  [] = write_header_file_from_inc(data_name, data_format, ...
%                 data_length_name, header_name, n, filename)
%
% Inputs:
%    data_name - Name of data array
%    data_format - Data format (float, double, q15, et.c.)
%    data_length_name - Name of data length #define
%    header_name - Name of header file #ifndef
%    n - Number of coefficients in .inc-file
%    filename - Name of output .h file and input .inc file WITHOUT file
%               extension
%
% Outputs:
%    None
%
% Example: 
%    % Write data to .inc file
%    write_float_to_inc([1:Ncoefs]./Ncoefs, pwd, 'a_filter.inc');
%    % Write to header file
%    write_header_file_from_inc('pdAFilterCoefs', 'double',...
%      'A_FILTER_LENGTH', '__A_FILTER_H', Ncoefs, 'a_filter');
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none

% Author - Markus Borgh
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2010-Oct-25
% Last modification - 2010-Oct-25

%------------- BEGIN CODE --------------


% ---< Read from .inc file >---
fid = fopen([filename,'.inc'],'r');
bin_data = fread(fid);
fclose(fid);


% ---< Write file header >---
% Open file for writing
fid = fopen([filename,'.h'],'w');

% Write #ifndef statement
textstr = (['#ifndef ', header_name]);
fprintf(fid, '%s\n', textstr);
% #define header statement
textstr = (['#define ', header_name]);
fprintf(fid, '\t%s\n\n', textstr);

% #define length of array
textstr = (['#define ', data_length_name, ' ', num2str(n)]);
fprintf(fid, '%s\n\n', textstr);

% Name of array and datatype
textstr = [data_format, ' ',data_name,'[', data_length_name, '] = {'];
fprintf(fid, '%s\n', textstr);


% ---< Write binary data >---
% Write data from include file
fwrite(fid, bin_data);


% ---< Write end of file >---
textstr = ['};'];
% Remove last '\n' and write ending '}'
fprintf(fid, '%s\n', textstr);

% Write end of #ifndef statement. Finish with new line
textstr = (['#endif // ', header_name]);
fprintf(fid, '\n%s\n', textstr);

% Close file
fclose(fid);
%------------- END OF CODE --------------