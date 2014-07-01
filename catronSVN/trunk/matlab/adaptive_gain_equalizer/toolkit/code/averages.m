function [A, An] = averages(X, alpha, beta, Lsub, Nsub)
%averages - Calculates short and long time average of the first Nsub subband data
%
% Syntax:  [A, An] = averages(X, alpha, beta, Lsub, Nsub)
%
% Inputs:
%    X - Subband data matrix, every row contains the data for that subband
%    alpha - Rise/fall parameter for the short time average
%    beta - Rise/fall parameter for the long time average
%    Lsub - Length of subbands
%    Nsub - Number of subbands
%
% Outputs:
%    A - Short time average
%    An - Long time average
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
% Last modification - 2009-10-28

%------------- BEGIN CODE --------------

% Input data fix
X = X(1:Nsub, 1:Lsub);

% Allocation
An = zeros(Nsub, Lsub);
A = zeros(Nsub, Lsub);

% Absolute value
Xabs = abs(X);

% Some special case here
n=1;
k=1;
A(:,n) = Xabs(:,n);

% The rest of the data
for n=2:Lsub
    % Step through subbands k
	for k=1:Nsub

		% i is (decimated) sample index
		% k is subband index

        % Short term average
        A(k,n) = (1-alpha)*A(k,n-1) + alpha*Xabs(k,n);
        
        % Noise level estimate
        if(A(k,n)>An(k,n-1) && n>500)
            An(k,n) = (1+beta)*An(k,n-1);
        else
            An(k,n) = A(k,n);
        end
	end

end


% Fix for first samples of blocks so that plotting looks better
F = 10;
for k=1:Nsub
    An(k,1:F) = An(k,F);
    A(k,1:F) = A(k,F);
end

%------------- END OF CODE --------------