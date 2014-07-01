function [Y] = hermitian_symmetry(X, Lsub, Nsub)
%hermitian_symmetry - Calculates the correct values for the negative
%frequencies of subband data by using the hermitian symmetry owing to the
%real input data.
%
% Syntax:  [Y] = hermite_symmetry(X)
%
% Inputs:
%    X - Subband data, each row corresponds to a certain subband
%    Lsub - Length of each subband
%    Nsub - Number of subbands
%
% Outputs:
%    Y - Negative frequencies are correct here
%
% Example: 
%    Line 1 of example
%    Line 2 of example
%    Line 3 of example
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: none

% Author - Markus Borgh
% E-mail - markus.borgh@limestechnology.com
% Website - www.limestechnology.com
% Copyright - Limes Technology AB
% Created - 2009-10-27
% Last modification - 2009-10-27

%------------- BEGIN CODE --------------
Lx = size(X,1); % How many rows exists?
Y = [X; zeros(Nsub-Lx, Lsub)]; % Fill upp with zeros

% Subband processing
for n=1:Lsub
	% Negative frequencies
	for k=Nsub/2+2:Nsub
		% Hermitian symmetry owing to real input signals
		Y(k,n) = X(Nsub+2-k,n)';
    end
end

%------------- END OF CODE --------------