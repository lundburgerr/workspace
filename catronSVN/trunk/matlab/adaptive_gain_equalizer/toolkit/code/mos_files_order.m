% Calculates random selection of files for listening tests. Returns an
% array for each testperson to which file (1-8 or A-H) that should be used.

% Author - Markus Borgh
% E-mail - markus.borgh@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2011-May-27
% Last modification - 2011-May-27

close all
clear all

% Number of files in CMOS (CCR) testing
nf = 8;
% name of files
name = [['1':'8'];['A':'H']]';

% Number of testpersons
ntp = 24;

for n=1:ntp
    % Random (uniform distribution) order. This tells what column should be
    % used
    ind = floor(rand(1,nf)*2+1);

    % What files should be used?
    for i=1:nf
        use(i) = name(i,ind(i));
    end

    disp(['The files that should be used for testperson ',num2str(n),' are: ',use])
end