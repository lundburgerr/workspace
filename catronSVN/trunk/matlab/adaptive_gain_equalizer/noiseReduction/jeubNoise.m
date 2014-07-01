function [s_hat, s_hatpf, db_drop, meanDiff, Gpfnrj] = jeubNoise(x, y, Px, Py, scalefactor, NFFT, blocksize, fs)
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

%TODO: Px and Py should not need to be ins NPSD length, or take NFFT as
%parameter
if length(x)~=length(y)
    error('x and y needs to be of same length');
end

%Test different values for blocksize and NFFT to see if it works still
N = length(x);
%blocksize = 320;
%NFFT = 512;
fn = fs/2;
filtersize = 40;
if mod(NFFT,2) == 0
    NPSD = NFFT/2+1;
else
    NPSD = (NFFT+1)/2;
end

if length(Px)~=NPSD || length(Py)~=NPSD
    error('Px and Py needs to be of length NFFT/2+1 if NFFT is even and (NFFT+1)/2 if NFFT is odd!');
end

%Parameter values, constants
PHI_MIN = 0.2;
PHI_MAX = 0.8;
GAMMA = 6;
ALPHA1 = 0.9; %TODO: what is this used for?
ALPHA2 = 0.9;
ALPHA3 = 0.8;
ALPHAnn = 0.9; %TODO: what is this used for?

ZETAthr = 0.4; %Smoothing threshold for PF filter [9]
PSIMIN = 10; %TODO: scaling factor for PF filter
F0 = 1000; %smooth out after F0Hz

LAMBDA = 0.01; %Adaptation speed for Px,Py
GAINMIN = 10^(-12/20);
SIZEADAP = 0.15; %maximum adaptation -1 of size
MALENGTH = 9; %Probably should be an odd integer
GADAPT = 1; %Speed of adaptation for the gain
SCALEADAPT = 0.03;

FALL = 0.85;
RISE = 3;

%The algorithm
M = floor(length(x)/(blocksize/2));
phi_nn = zeros(NPSD, 1); %TODO: better initial condition?
s_hat = zeros(N, 1);
s_hatpf = zeros(N, 1);
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
G = ones(NPSD, 1);
Gpf = ones(NPSD, 1);

meanDiff = [];
Gpfnrj = [];
Gain = ones(NPSD, 1);
%scalefactor = 1;
t = 1:blocksize;
% phi1 = pwelch(x(t), [], [], NFFT); %TODO: Calculated over recursive time ALPHA1 and frame overlap and window etc. etc.
% phi1 = phi1.*10.^(-Px/20);%TODO: Should it be phi - phiDiff or just phi, seems ot make it better
% phi2 = pwelch(y(t), [], [], NFFT);
% phi2 = phi2.*10.^(-Py/20);
% 
% Px_smooth = Px;
% Py_smooth = Py;
% phi1_smooth = pwelch(x(t), [], [], NFFT); %TODO: Calculated over recursive time ALPHA1 and frame overlap and window etc. etc.
% phi1_smooth = phi1_smooth.*10.^(-Px_smooth/20);%TODO: Should it be phi - phiDiff or just phi, seems ot make it better
% phi2_smooth = pwelch(y(t), [], [], NFFT);
% phi2_smooth = phi2_smooth.*10.^(-Py_smooth/20);
phi1 = pwelch(x(t), [], [], NFFT);
phi2 = pwelch(y(t), [], [], NFFT);
phi1_smooth = phi1;
phi2_smooth = phi2;
P = 20*log10(phi2)-20*log10(phi1);
P_smooth = P;
for m=1:M-1

    t = 1+(m-1)*blocksize/2:(m+1)*blocksize/2;
    x1 = x(t).*w;
    x2 = y(t).*w;%*scalefactor;
    X1 = fft(x1, NFFT);
    X2 = fft(x2, NFFT);
    X1_mag = abs(X1).^2/NFFT; %Is this the best scaling?
    X1_mag_smooth = filter(ones(MALENGTH,1)/MALENGTH, 1, X1_mag);
    X2_mag = abs(X2).^2/NFFT;
    X2_mag_smooth = filter(ones(MALENGTH,1)/MALENGTH, 1, X2_mag);
    
    X1_mag = X1_mag(1:NPSD);
    X1_mag_smooth = X1_mag_smooth(1:NPSD);
    X2_mag = X2_mag(1:NPSD).*10.^(-P/20);
    X2_mag_smooth = X2_mag_smooth(1:NPSD).*10.^(-P_smooth/20);
    
    phi1 = (1-ALPHA1)*phi1 + ALPHA1*X1_mag;%TODO: Should it be phi - phiDiff or just phi, seems ot make it better
    phi2 = (1-ALPHA1)*phi2 + ALPHA1*X2_mag;
    phi1_smooth = (1-ALPHA1)*phi1_smooth + ALPHA1*X1_mag_smooth;%TODO: Should it be phi - phiDiff or just phi, seems ot make it better
    phi2_smooth = (1-ALPHA1)*phi2_smooth + ALPHA1*X2_mag_smooth;
    
    phi_pld_smooth = phi1_smooth-phi2_smooth;
    phi_pld_smooth(phi_pld_smooth<0) = 0;
        %figure(1); plot(phi_pld_smooth); hold on; plot(phi1_smooth, 'g'); plot(phi2_smooth, 'r'); hold off;
        
    
    phi_pld = phi1-phi2;
%         figure(2); plot(phi_pld); hold on; plot(phi1, 'g'); plot(phi2, 'r'); hold off;
    %phi_pld_smooth(phi_pld_smooth<0) = 0;
    phi_pld(phi_pld<0) = 0;
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    phi_pldne = phi_pld_smooth./(phi1_smooth+phi2_smooth+eps);
    %phi_pldne(phi_pldne<0) = 0;
    %phi_pldne = phi_pld./(phi1+phi2+eps);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    meanDiff = [meanDiff; mean(phi_pldne)];   
%     figure(12); hold on; plot(t, x(t));
%     figure(13); hold off; plot(phi1, 'r'); hold on; plot(phi2, 'b'); plot(phi_pld, 'g');
%     figure(14); hold off; plot(phi_pldne); ylim([0,1]);
    %Estimating phi_nn
    NSA = 0; %Number of bins that indicate to scale adapt
    SPEECH = false;
    for n=1:NPSD
%         if phi_pldne(n) < -PHI_MAX
%             NSA = NSA - (log(1+phi_pldne(n))/log(0.2))^2;
%             SPEECH = true;
        if phi_pldne(n) < PHI_MIN %&& phi_pldne(n) > -PHI_MIN %No speech
            phi_nn(n) = ALPHA2*phi_nn(n) + (1-ALPHA2)*X1_mag_smooth(n);
            %phi_nn(n) = ALPHA2*phi_nn(n) + (1-ALPHA2)*X1_mag(end+1-n);%TODO: from where should I calculate X_mag?
            %phiDiff(n) = phi1(n)-phi2(n);
            NSA = NSA+1; %TODO: Take into consideration that phi_pld could have been negative, then
        elseif phi_pldne(n) > PHI_MAX %speech
            phi_nn(n) = phi_nn(n); %Keep old value
            NSA = NSA - (log(1-phi_pldne(n))/log(0.2))^2;
            SPEECH = true;
            %NSA = NSA-1;
            %phiDiff(n) = phiDiff(n);
        else %sorta no speech
            phi_nn(n) = ALPHA3*phi_nn(n) + (1-ALPHA3)*X2_mag_smooth(n);
            %phi_nn(n) = ALPHA3*phi_nn(n) + (1-ALPHA3)*X2_mag(end+1-n);
            NSA = NSA+0.5; %TODO: Add 0.5 instead?
        end
    end
    
    if NSA < 0 
        NSA = 0;
    end
    PSI_SCALE = PSIMIN;%NSA/NPSD*100;
%     %PSI_SCALE = 10;
%     if PSI_SCALE < PSIMIN %|| SPEECH %TODO: in this case this is only SPEECH
%         PSI_SCALE = PSIMIN;
%     end
    if NSA > NPSD/3 && ~SPEECH
        P_new = 20*log10(X2_mag)-20*log10(X1_mag);
        P = (1-LAMBDA)*P + LAMBDA*P_new;
        P(P<-20) = -20;
        
        P_smooth_new = 20*log10(X2_mag_smooth(1:NPSD))-20*log10(X1_mag_smooth(1:NPSD));
        P_smooth = (1-LAMBDA)*P_smooth + LAMBDA*P_smooth_new;
        P_smooth(P_smooth<-20) = -20;
    end
    %scaling = mean(abs(x1))/mean(abs(x2));

    %TODO: If speech mic starts out much more stronger than noise mic. This
    %will never adapt. To fix this, check if NSPEECH is to big or NSA to
    %small.
    if NSA > NPSD/3 %~SPEECH %TODO: Maybe have NSA > NPSD/3 aswell here?
        scaling = sqrt(sum(X1_mag)/sum(X2_mag));
        %speed = NSA/NPSD;
        %if speed(end) > 0.5
        %relScale = (1-speed(end))*scalefactor + speed(end)*scaling;
        if scaling > 1+SIZEADAP
            scaling = 1+SIZEADAP;
        elseif scaling< 1-SIZEADAP
            scaling = 1-SIZEADAP;
        end
        scaling = (1-SCALEADAPT)*1+SCALEADAPT*scaling;
        scalefactor = scalefactor*scaling;
    end

    
    %Calculate gain filter
    G = phi_pld./(phi_pld + GAMMA*phi_nn+eps);
    for n=1:NPSD
        if Gain(n) > G(n)
            Gain(n) = Gain(n)*FALL;
            Gain(n) = max(G(n), Gain(n));
        else
            Gain(n) = Gain(n)*RISE;
            Gain(n) = min(G(n), Gain(n));
        end
    end
    Gain(Gain<GAINMIN) = GAINMIN;
    Gain = musicSupp(Gain, X1(1:NPSD), ZETAthr, PSI_SCALE, round(F0/fn*NPSD));
    Gpfsym = [Gain(1:end-1); flipud(Gain(2:end))];
     %figure(11); hold on; plot(t, y(t)*scalefactor, 'b'); plot(t, x(t), 'r')
     %figure(22); plot(Gpf); hold off;
     %figure(33); plot(X1_mag_smooth); hold on; plot(X2_mag_smooth, 'r'); hold off; ylim([0, 1]*10^-3);
%     hold off; figure(22); plot(Gpf); ylim([0,1]);
     %pause(0.2);
    
    stmppf = ifft(Gpfsym.*X1);
    s_hatpf(t) = s_hatpf(t) + stmppf(1:blocksize);

    t_begin = 1+(m-1)*blocksize/2:m*blocksize/2;
    Gpfnrj = [Gpfnrj; mean(Gain)];
    Pow_after(m) = sum(abs(s_hatpf(t_begin)).^2);
    Pow_init(m) = sum(abs(x(t_begin)).^2);
    db_drop(m) = 10*log10(Pow_after(m)/Pow_init(m));
end
disp(toc/(M-1));
  