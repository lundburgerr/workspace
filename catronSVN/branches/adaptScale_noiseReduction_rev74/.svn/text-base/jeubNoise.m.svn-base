function [s_hat, s_hatpf, db_drop, diffdiff] = jeubNoise(x, y, fs)
%[s_hat, s_hatpf] = PSDdiff_jeubNoise(x, y, fs)
%s_hat      - the noise reduced signal
%s_hatpf    - the noise reduced and musical noise removed signal
%
%x          - headset mic closest to usable signal source
%y          - mic collecting noise.
%fs         - sampling frequency
%Note that x and y is assumed to have the same frequency response and of
%same magnitude
%
%This algorithm is based on the paper "Noise reduction for dual-microphone
%mobile phones exploiting power level differences", Jeub etc.
%but also using smoothing technique on higher frequencies from paper
%"Efficient musical noise suppression for speech enhancement systems"
%Esch, Vary.

% Author - Robin Lundberg
% E-mail - robin.lundberg@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2013-Jun-06
% Last modification - 2013-Jun-06

if length(x)~=length(y)
    error('x and y needs to be of same length');
end

%Test different values for blocksize and NFFT to see if it works still
N = length(x);
blocksize = 320;
NFFT = 512;
fn = fs/2;
if mod(NFFT,2) == 0
    NPSD = NFFT/2+1;
else
    NPSD = (NFFT+1)/2;
end


%Parameter values, constants
PHI_MIN = 0.2;
PHI_MAX = 0.8;
GAMMA = 4;
ALPHA1 = 0.9; %TODO: what is this used for?
ALPHA2 = 0.9;
ALPHA3 = 0.8;
ALPHAnn = 0.9; %TODO: what is this used for?

ZETAthr = 0.4; %Smoothing threshold for PF filter [9]
PSI_SCALE = 10; %TODO: scaling factor for PF filter
F0 = 1000; %smooth out after 1000Hz

LAMBDA = 0.9; %Adaptation speed for phidiff, between 0 and 1.
GAINMIN = 10^(-20/20);
SIZEADAP = 0.05;

%The algorithm
M = floor(length(x)/(blocksize/2));
phi_nn = zeros(NPSD, 1); %TODO: better initial condition?
s_hat = zeros(N, 1);
s_hatpf = zeros(N, 1);
%TODO: check if this adds to 1 for different blocksizes
if mod(blocksize,2) == 0
    w = hann(blocksize, 'periodic');
else
    w = hann(blocksize, 'symmetric');
end

tic
E_after = [];
E_init = [];
db_drop = [];
%[yhat, hhat, e] = nlms(y(1:blocksize/2), x(1:blocksize/2), filtersize, 0.15);
%x1_old = x(1:blocksize/2);
%x2_old = filter(hhat, 1, y(1:blocksize/2));
%phiDiff = pwelch(x(1:4), [], [], NFFT) - pwelch(y(1:blocksize/2), [], [], NFFT);

meanDiff = [];
diff_old = zeros(NPSD, 1);
diffdiff = [];
phiDiff = zeros(NPSD, 1);
scalefactor = 1;
for m=1:M-1
    t = 1+(m-1)*blocksize/2:(m+1)*blocksize/2;
    %[yhat, hhat, e] = nlms(y(t), x(t), 40, 0.15);
    x1 = x(t).*w;
    x2 = y(t).*w*scalefactor;

    phi1 = pwelch(x1, [], [], NFFT); %TODO: Calculated over recursive time ALPHA1 and frame overlap and window etc. etc.
    phi1 = phi1 - phiDiff; %TODO: Should it be phi - phiDiff or just phi, seems ot make it better
    phi2 = pwelch(x2, [], [], NFFT);

    
    %phi_12 = cpsd(x1,x2, [], [], NFFT); %Cannot have cpsd on x1 and x2
    %phi_12 = abs(phi_12); %TODO: Should I really abs this? -> yeah
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %%%% Potentially a good vad %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%     phi_pld = phi1-phi2;
%     scalefactorDD = mean(abs(phi_pld))/( mean(abs(diff_old)) + eps);
%     diffdiff = [diffdiff; sum(abs(phi_pld - diff_old*scalefactorDD))];
%     diff_old = phi_pld;
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    phi1(phi1<0) = 0;
    phi_pld = phi1-phi2;
    %phi_pld = abs(phi_pld);
    phi_pld(phi_pld<0) = 0; %Is this better than abs()-ing it?
    phi_pldne = phi_pld./(phi1+phi2+eps);
    meanDiff = [meanDiff; mean(phi_pldne)];
    %TODO: Adaptation of phi_diff could potentially be in loop below
    %TODO: Use diffdiff instead of phi_pldne to determine
    %diffdiff_ne = diffdiff(end)/(sum(abs(phi_pld) + abs(diff_old*scalefactorDD)) + eps); % This is a very bad normalization, needs to be updated
    if mean(phi_pldne) < PHI_MIN
    %if diffdiff(end) < 2*10^-3
        phiDiff = (1-LAMBDA)*phiDiff + LAMBDA*(phi1-phi2);
    end
    phi1 = pwelch(x1, [], [], NFFT);
    phi1 = phi1 - phiDiff;
    phi_pld = phi1-phi2;
    phi_pld(phi_pld<0) = 0;
    phi_pldne = phi_pld./(phi1+phi2+eps);
    
    %Estimating phi_nn
    X1 = fft(x1, NFFT);
    X2 = fft(x2, NFFT);
    X1_mag = abs(X1).^2/NFFT; %Is this the best scaling?
    X2_mag = abs(X2).^2/NFFT;
    %scaling = mean(abs(x1))/mean(abs(x2));
    NnoSpeech = 0;
    for n=1:NPSD
        if phi_pldne(n) < PHI_MIN %No speech
            phi_nn(n) = ALPHA2*phi_nn(n) + (1-ALPHA2)*X1_mag(end+1-n);%TODO: from where should I calculate X_mag?
            %phiDiff(n) = phi1(n)-phi2(n);
            NnoSpeech = NnoSpeech+1;
        elseif phi_pldne(n) > PHI_MAX %speech
            phi_nn(n) = phi_nn(n); %Keep old value
            NnoSpeech = NnoSpeech+1; %TODO: Add 0.5 instead?
            %phiDiff(n) = phiDiff(n);
        else %sorta no speech
            phi_nn(n) = ALPHA3*phi_nn(n) + (1-ALPHA3)*X2_mag(end+1-n);
        end
    end
    scaling = mean(abs(x1))/mean(abs(x2));
    speed = NnoSpeech/NPSD;
    %if speed(end) > 0.5
    %relScale = (1-speed(end))*scalefactor + speed(end)*scaling;
    if scaling > 1+SIZEADAP
        scaling = 1+SIZEADAP;
    elseif scaling< 1-SIZEADAP
        scaling = 1-SIZEADAP;
    end
    scaling = (1-speed)*1+speed*scaling;
    scalefactor = scalefactor*scaling;
   % end
  
    %Calculate gain filter G 
    G = phi_pld./(phi_pld + GAMMA*phi_nn+eps);
    G = filter(ones(9,1)/9, 1, G); %smooth the filter
    G(G<GAINMIN) = GAINMIN;

    Gsym = [G(1:end-1); flipud(G(2:end))];
    %TODO: How should the Gsym be created??
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    
    %Gsym = [G(1); G(1:end-1); flipud(G)];
    stmp = ifft(Gsym.*X1);
    s_hat(t) = s_hat(t) + stmp(1:blocksize);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%    
    Gpf = musicSupp(G, X1(1:NPSD), ZETAthr, PSI_SCALE, round(F0/fn*NPSD));
    Gpfsym = [Gpf(1:end-1); flipud(Gpf(2:end))];
    
    stmppf = ifft(Gpfsym.*X1);
    s_hatpf(t) = s_hatpf(t) + stmppf(1:blocksize);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%     if m>M*0.5
%         figure(11); plot(t, x(t), 'r'); hold on; plot(t, real(s_hat(t)), 'b');
%         figure(12); plot(t, x(t), 'r'); hold on; plot(t, y(t)*scalefactor, 'b'); plot((m-1)*blocksize/2, speed(end)/10)
%     end
    E_after(m) = mean(abs(s_hat(t)).^2);
    E_init(m) = mean(abs(x(t)).^2);
    db_drop(m) = 20*log10(E_after(m)/E_init(m));
end
disp(toc/(M-1));

%E_init = sum(x.^2)
%E_after = sum(real(s_hat).^2)
figure; plot(db_drop);
figure; plot((1:N)/N, x, 'r'); hold on;
           plot((0:(N-1))/N, s_hat, 'b');
           plot((0:(N-1))/N, s_hatpf, 'k'); 
           %plot((1:(M-1))/(M-1), diffdiff, 'g');
           %plot((0:(M-2))/(M-1), meanDiff/10, 'g');
           hold off;
            
%sound(s_hat, fs)
%Hf = fdesign.lowpass('N,Fc',100,0.4);
%Hd1 = design(Hf,'window','window',@hamming,'SystemObject',true);
%filter(Hd1.coeffs.Numerator, 1, s_hat)
  