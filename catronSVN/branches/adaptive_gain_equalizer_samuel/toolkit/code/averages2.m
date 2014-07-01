function [A, An] = averages2(X, alpha, beta, Lsub, Nsub, block_nr, block_size, comp_factor, noise_thresh)
%averages2 - Calculates short and long time average of the first Nsub
%subband data using the method from FENIX
%
% Syntax:  [A, An] = averages2(X, alpha, beta, Lsub, Nsub, block_nr, block_size, comp_factor)
%
% Inputs:
%    X - Subband data matrix, every row contains the data for that subband
%    alpha - Rise/fall parameter for the short time average
%    beta - Rise/fall parameter for the long time average
%    Lsub - Length of subbands
%    Nsub - Number of subbands
%    block_nr - Number of blocks used in noise floor estimation
%    block_size - Accumulation length of blocks for noise floor estimation
%    comp_factor - Factor to compensate difference between min- and mean-value in noise level estimation
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
% Created - 2009-10-28
% Last modification - 2009-11-10

%------------- BEGIN CODE --------------

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
            bUpdateNoiseLevel = false;
        end

        % Use instant fall (minimum statistics)
        An(k,n) = min(mini(k), An(k,n-1));

        % Update noise level estimate
        if bUpdateNoiseLevel

            % Speech detection
            if( maxi(k)>noise_thresh*mini(k) ...
                    && maxi(k)>0.0e-2 ...%0.3e-2 ...   (noise_thresh*4+1)
                    )
                %                     || maxi(k)>1 ...
                % No noise estimation increase
				bUpdateNoiseLevel = false;
            end
        end
        maxi_save(k,n) = maxi(k);
    end
    
    if(bUpdateNoiseLevel)
        for k=1:Nsub
            % Slow rise
            An(k,n) = (1-beta)*An(k,n-1) + beta*mini(k);
            
            % No speech detected, store this information... If speech
            % detected, value is zero.
            speech(k,n:n+block_size-1) = An(k,n);
        end
    end

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

% Take mean value, not acccumulated
An = An./block_size;
speech = speech./block_size;

% Compensate with factor to adjust difference between mean and min
An = An*comp_factor;
speech = speech*comp_factor;

speech = speech(:,1:Lsub);

save speech.mat speech maxi_save

%------------- END OF CODE --------------