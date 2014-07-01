function [h] = read_hex_from_inc(name)
%READ_HEX_FROM_INC - Opens an .inc file and read hex data to vector. Output
% is rescaled to interval [-1,1)
%
% Syntax:  [h] = read_hex_from_inc(name)
%
% Inputs:
%    name - Name of .inc file. Data in file is assumed to be in the
%           interval [-32768,32767] and in hex format
%
% Outputs:
%    h - Vector with data rescaled to interval [-1,1)
%
% Example: 
%    h = read_hex_from_inc('tx_filter1.inc');
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none

% Author - Markus Borgh
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2009-09-09
% Last modification - 2010-10-22

% name = 'ls_filter_16kHz.inc';
fid = fopen(name, 'r');

f = fread(fid,'uint8=>char');
%f

L = length(f);

% Init
g = cell(floor(L/4),1);
bStoreValue = false;
j=1;

% Search through data and store
for(i=1:L)
    % Should we stop to store values ?
    if(f(i)==',')
        bStoreValue = false;
        % Move to next char matrix
        j = j+1;
    end
    
    % Store values in char matrix
    if(bStoreValue)
        g{j} = [g{j}, f(i)];
    end
    
    % Should the following values be stored?
    if(f(i)=='x')
        bStoreValue = true;
    end
    
end

% Only keep the first four char in the last matrix
g{j} = g{j}(1:4);

% Convert to dec
h = zeros(j,1);
for(i=1:j)
    h(i) = hex2dec(g{i});
    h(i) = h(i)./32768;
    if(h(i)>1)
        h(i) = h(i)-2;
    end
end

fclose(fid);