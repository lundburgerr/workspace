function [ fixedSound ] = subMultiply( x, y, xs, ys )
%SUBMULTIPLY Summary of this function goes here
%   x, voice mic signal
%   y, noice mic signal
%
%   (optional)
%   xs, a sample of the voice signal. Is used to train the algorithm
%   ys, a sample of the noice signal. Is used to train the algorithm

addpath ./toolkit/code;
addpath ./filterbank1;

algoritmFs = 8000;
Sound = x;
Noise = y;

use64 = false;

if use64 %warning! this mode haven't been tested lately 
    % Subband filterbank settings
    M = 64; % Number of subbands (orig. 64)
    N = 256; % Number of prototype filter coefficients (orig. 256)
    R = M/2; % Decimation ratio
    p = IterLSDesign(N, M, R);
    %figure; freqz(p, 1, 4096, 16000);

    S_vec = SubAnal(Sound, N, M, R, p);
    N_vec = SubAnal(Noise, N, M, R, p);
    
    speechGrowthFactor = 0.98;
    attackFactor = 1.095;
    releaseFactor = 0.975;
    

    G = zeros(size(S_vec));
    speechThreshold = 2;
    noGain = 1.25;
    gainSpread = 1.25;
    minGdB = -18; %the minimum gain
    minG = 10^(minGdB/20);
    alpha = 0.000;  

    %no speech (NS) values
    NSnoGain = 5;
    NSgainSpread = 3.75;
    NSminGdB = -24;
    NSminG = 10^(NSminGdB/20);
    NSalpha = 0.2;
    %
else
    % Subband filterbank settings
    M = 2*64; % Number of subbands (orig. 64)
    N = 2*256; % Number of prototype filter coefficients (orig. 256)
    R = M/2; % Decimation ratio
    p = IterLSDesign(N, M, R);
    %figure; freqz(p, 1, 4096, 16000);

    S_vec = SubAnal(Sound, N, M, R, p);
    N_vec = SubAnal(Noise, N, M, R, p);

    speechGrowthFactor = 0.99;
    attackFactor = 1.2;
    releaseFactor = 0.95;
    
    G = zeros(size(S_vec));
    speechThreshold = 2;
    noGain = 1.25;
    gainSpread = 1.25;
    minGdB = -12; %the minimum gain
    minG = 10^(minGdB/20);
    alpha = 0.000;  

    %no speech (NS) values
%     NSnoGain = 5;
%     NSgainSpread = 3.75;
%     NSminGdB = -24;
%     NSminG = 10^(NSminGdB/20);
    NSnoGain = noGain;
    NSgainSpread = gainSpread;
    NSminGdB = minGdB;
    NSminG = 10^(NSminGdB/20);
    NSalpha = 0.2;
    %
end

d = ones(size(S_vec,1),1);
%if we have a noice sample, we use that to get a good startvalue for d
if nargin==4
    Ss_vec = SubAnal(xs, N, M, R, p);
    Ns_vec = SubAnal(ys, N, M, R, p);
    
    for k=1:M
        ss = abs(Ss_vec(k,:));
        ns = abs(Ns_vec(k,:));
        
        d(k) = (ss*ns')/(ns*ns');
    end
end

speech_vec = ones(1,size(S_vec,2));
diff_vec = ones(1,R*size(S_vec,2));
for t=1:size(S_vec,2)
    h = abs(S_vec(:,t));
    n = abs(N_vec(:,t));
    hn = n.*d; %estimated noice
    k = h./(hn+eps); %used to update d
    diff=(h-hn)/mean(hn); %used to calculate gain
    diff_vec(1+(t-1)*R:t*R) = max(abs(diff)); %stored for debugging
    
    %VAD
    speech = sum(diff>speechThreshold)>0;
    if (t>1) 
        speech_vec(t) = min(1,speech + speechGrowthFactor*speech_vec(t-1));
    else
        speech_vec(t) = speech;
    end
    useSpeechValues = speech_vec(t)>0.2;
    %
    
    % Update d
    if (useSpeechValues)
        d = (1-alpha)*d + alpha*k;
    else
        d = (1-NSalpha)*d + NSalpha*k;
    end
    %

    %calculate gain
    if (useSpeechValues)
        m = 1+(minG-1)*noGain/gainSpread;
        k = (1-minG)/gainSpread;
        g=m+k*diff;
        ind = find(g<minG);
        g(ind) = minG;        
    else
        m = 1+(NSminG-1)*NSnoGain/NSgainSpread;
        k = (1-NSminG)/NSgainSpread;
        g=m+k*diff;
        ind = find(g<NSminG);
        g(ind) = NSminG;
    end
    %

    %never have gain>1
    ind = find(g>1);
    g(ind) = 1;
    %
    
    %attack and release, mostly untested. 
    if (t==1)
        G(:,t) = g; 
    else
        gMultiplier = ones(size(g));
        gOld =  G(:,t-1);
        
        indices = find(gOld<g);
        gMultiplier(indices) = attackFactor;
        
        indices = find(gOld>g);
        gMultiplier(indices) = releaseFactor;
        
        newG = gOld.*gMultiplier;
        
        indices = find((newG<g & gOld>g) | (newG>g & gOld<g));
        newG(indices) = g(indices);

        G(:,t) = newG;
    end
    %
      
end

S_vec_nod = S_vec.*G;
%N_vec_nod = N_vec.*G;

SIMU_LENGTH = length(Sound);
S_out = SubSynt(S_vec_nod, N, SIMU_LENGTH-N, R, p);
%N_out = SubSynt(N_vec_nod, N, SIMU_LENGTH-N, R, p);
%G_out = SubSynt(G, N, SIMU_LENGTH-N, R, p);

% Fill up with zeros to get same length as other vectors
S_out = fill_up_with_const(S_out, SIMU_LENGTH, 0);
%N_out = fill_up_with_const(N_out, SIMU_LENGTH, 0);
%G_out = fill_up_with_const(G_out, SIMU_LENGTH, 0);

T = (0:length(Sound)-1)*1/algoritmFs;
t = (0:length(speech_vec)-1)*T(end)/(length(speech_vec)-1);
figure
hold on
plot(t,max(Sound)*(speech_vec>0.2),'g');
plot(T,Sound,'r'); 
plot(T,S_out,'k');
hleg = legend('VAD','Original','Fixed');
set(hleg,'Location','SouthEast');

fixedSound = S_out;

end

