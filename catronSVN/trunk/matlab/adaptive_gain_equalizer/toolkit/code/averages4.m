function [A, An] = averages4(X, alpha, beta, Lsub, Nsub, block_nr, block_size, noise_thresh)
%averages2 - Calculates short and long time average of the first Nsub
%subband data using the modified FENIX method
%
% Syntax:  [A, An] = averages4(X, alpha, beta, Lsub, Nsub, block_nr, block_size)
%
% Inputs:
%    X - Subband data matrix, every row contains the data for that subband
%    alpha - Rise/fall parameter for the short time average
%    beta - Rise/fall parameter for the long time average
%    Lsub - Length of subbands
%    Nsub - Number of subbands
%    block_nr - Number of blocks used in noise floor estimation
%    block_size - Accumulation length of blocks for noise floor estimation
%    noise_thresh - Minimum max/min ratio to detect speech
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
% Created - 2009-11-10
% Last modification - 2010-02-17

%------------- BEGIN CODE --------------

CONNECT_SUBBANDS = 1;
% block_nr
% block_size

% Input data fix
X = X(1:Nsub, 1:Lsub);

% Allocation
An = zeros(Nsub, Lsub);
A = zeros(Nsub, Lsub);
mini = zeros(Nsub, 1);
maxi = zeros(Nsub, 1);
block = zeros(Nsub, block_nr);
speech = zeros(Nsub, Lsub);
maxi_save = zeros(Nsub, Lsub);
S = zeros(Nsub, Lsub);

% Absolute value
Xabs = abs(X);

% Initialization
n=1;
A(:,n) = Xabs(:,n);
% Noise level
block_index = 1;
block(:,block_index) = block(:,block_index) + Xabs(:,n);
nb = 2; % Next sample


% The rest of the data
for n=2:Lsub

	bUpdateNoiseLevel = true;

	% Step through subbands k
    for k=1:Nsub

        % i is (decimated) sample index
        % k is subband index

        % --- Short term average
        A(k,n) = (1-alpha)*A(k,n-1) + alpha*Xabs(k,n);

        % --- Noise level estimate
        % Accumulate blocks
        block(k,block_index) = block(k,block_index) + Xabs(k,n);
        % If block is full, find minimum and maximum value
        if(nb == block_size)
            mini(k) = min(block(k,:));
            maxi(k) = max(block(k,:));
        else
            % If this line is included, the beta will be block_size times slower
            % bUpdateNoiseLevel = false;
        end
        
        % Voice activity detection statistic
        S(k,n) = maxi(k)/(mini(k)+eps);
        
        % Update noise level estimate
        %if bUpdateNoiseLevel

            % Speech detection
            %if( maxi(k)>noise_thresh*(eps + mini(k)) )
            if( S(k,n) >= noise_thresh )
                % No noise estimation increase
				bUpdateNoiseLevel = false;
                speech(k,n) = 1;
            end
        %end
        maxi_save(k,n) = S(k,n); %maxi(k)/(mini(k)+eps);
    end
    
    for k=1:Nsub
        if(CONNECT_SUBBANDS)
            if(n>500 && bUpdateNoiseLevel)
                An(k,n) = (1+beta)*An(k,n-1);
            elseif(n==500)
                % Init
                An(k,n) = A(k,n);
            else
                % Don't update level
                An(k,n) = An(k,n-1);
            end
            % Avoid overflow
            An(k,n) = min(An(k,n), A(k,n));
        else
            if(n<=500)
                % Init
                An(k,n) = A(k,n);
            elseif(A(k,n) < An(k,n-1))
                % Drop down to short time average
                An(k,n) = A(k,n);
            elseif(speech(k,n))
                % Don't update level
                An(k,n) = An(k,n-1);
            else
                % Slow increase
                An(k,n) = (1+beta)*An(k,n-1);
            end
        end
    end

%     if(bUpdateNoiseLevel)
%         for k=1:Nsub
%             % No speech detected, store this information... If speech
%             % detected, value is zero.
%             speech(k,n:n+block_size-1) = An(k,n);
%         end
%     end
    
    % Increase counter
    nb = nb + 1;
    % If block is full move on to next block
    if(nb > block_size)
        nb = 1;
        block_index = block_index + 1;
        % If all blocks full, start over from first block
        if(block_index > block_nr)
            block_index = 1;
        end
        % Reset block
        block(:,block_index) = zeros(Nsub,1);
    end
    
end

% Fix for first block_nr of blocks
for k=1:Nsub
    An(k,1:block_nr*block_size) = An(k,block_nr*block_size+1);
end

speech4 = speech(:,1:Lsub);
maxi_save4 = maxi_save;

save speech4.mat speech4 maxi_save4

%------------- END OF CODE --------------