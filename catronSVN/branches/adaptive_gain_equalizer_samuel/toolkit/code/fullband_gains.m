function [G] = fullband_gains(Gs, Ls, Lt, Lfast, Ts)
%fullband_gains - Calculates the extra fullband gain that is applied to all
%subbands after subband gain is applied.
%
% Syntax:  [G] = fullband_gains(Gs, Ls, Lt, Lfast, Ts)
%
% Inputs:
%    Gs - Subband gain
%    Ls - Maximum allowed subband gain [dB]
%    Lt - Maximum allowed total gain [dB]
%    Lfast - Adapt fast to this level [dB]
%    Ts - Speech detection threshold [dB]
%
% Outputs:
%    G - Fullband gain
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
% Created - 2009-11-04
% Last modification - 2009-11-09

%------------- BEGIN CODE --------------

%damp_fast = 2;
gamma_fast = 0.9687; %0.95;  % FENIX: 0.9687
gamma_slow = 0.999; % FENIX: 0.9990
hold_time = 100;%2; % 100 <-> 200ms. Has got to be larger than 1 sample

% Number of subbands
Nsub = size(Gs,1);
% Data length, number of subband blocks
N = size(Gs,2);

% Memory allocation
G = ones(Nsub, N);

% Maximum allowed fullband gain
L = Lt-Ls;
% Fast adaption to this level
% Lfast = L-damp_fast;
Lfast = max(Lfast, 0);

disp(['fullband_gains: Speech detection threshold is ',num2str(Ts),' dB'])
disp(['fullband_gains: Fast damping down to ',num2str(Lfast),' dB'])


% Convert from dB
L = 10^(L/20);
Lt = 10^(Lt/20);
Ls = 10^(Ls/20);
Lfast = 10^(Lfast/20);
Ts = 10^(Ts/20);

% Init
hold_counter = 0;
m = 1;

% Step through all subband blocks
for(n=2:N)
    %[n,20*log10(max(Gs(:,n)))]
    % Check gain in all subbands for this time
    if(mean(Gs(:,n)')<Ts)
        %temp(m) = max(Gs(:,n)');
        %m = m+1;
        % Noise damping
        hold_counter = hold_counter - 1;
    else
        % Reset hold
        hold_counter = hold_time;
    end
    
    % Check how much the noise should be damped
    if(hold_counter <= 0)
        % Noise damping
        if(G(:,n-1)> Lfast*1.05) % Small increase since we have float. If fixtal this is not needed
            % Fast fall down to Lfast dB
            G(:,n) = gamma_fast*G(:,n-1) + (1-gamma_fast)*Lfast;
        else
            % Slow fall down to no gain
            G(:,n) = gamma_slow*G(:,n-1) + (1-gamma_slow)*1;
        end
        %hold_counter = 0; % Prevent overflow, not needed in Matlab
    else
        % Full gain
        G(:,n) = L;
    end
end

%save below temp

% Make this fullband gain a damping if only noise
% G = G./L;

%------------- END OF CODE --------------