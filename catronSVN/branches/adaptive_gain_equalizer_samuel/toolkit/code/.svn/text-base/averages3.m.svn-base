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

% Author - Christian Schüldt
% E-mail - christian.schuldt@limestechnology.com
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

alpha_d = 0.85;
alpha_p = 0.2;
betaN = 0.8;
gamma = 0.998;
nu = 0.7;
dirac = [ 2*ones(1, 12) 5*ones(1, Nsub-12) ];

P = zeros(1, Nsub);
Pmin = zeros(1, Nsub);
II = zeros(1, Nsub);
pp = zeros(1, Nsub);

% The rest of the data
for n=2:Lsub
	Pold = P;
    % Step through subbands k
	for k=1:Nsub

		% i is (decimated) sample index
		% k is subband index

        % Short term average
        A(k,n) = (1-alpha)*A(k,n-1) + alpha*Xabs(k,n);

		%%% Noise estimation %%%

		P(k) = nu*P(k) + (1-nu)*abs(Xabs(k,n)); % Eq. 2

		% Eq. 3
		if Pmin(k) < P(k)
			Pmin(k) = gamma*Pmin(k) + ((1-gamma)/(1-betaN))*(P(k) - betaN*Pold(k));
		else
			Pmin(k) = P(k);
		end

		% Eq. 5
		if P(k)/Pmin(k) > dirac(k)
			II(k) = 1; % Speech present
		else
			II(k) = 0; % Speech absent
		end

		pp(k) = alpha_p*pp(k) + (1-alpha_p)*II(k); % Eq. 6
		alpha_s = alpha_d + (1-alpha_d)*pp(k); % Eq. 7

		An(k,n) = alpha_s*An(k,n-1) + (1-alpha_s)*abs(Xabs(k,n)); % Eq. 8
	end

end

%------------- END OF CODE --------------