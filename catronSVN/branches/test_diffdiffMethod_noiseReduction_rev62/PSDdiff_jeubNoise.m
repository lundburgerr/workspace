function [s_hat, s_hatpf, db_drop, diffdiff, diffdiff_ne, pldne_diff] = PSDdiff_jeubNoise(x, y, fs)
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
PSI_SCALE = 1; %TODO: scaling factor for PF filter
F0 = 1000; %smooth out after 1000Hz

LAMBDA = 0.9; %Adaptation speed for phidiff, between 0 and 1.
NORM = 2*10^-5;

%Estimated coherence
dmic = 0.4; %distance in meter between mics in average
cs = 343.6; %speed of sound
f = (0:fn/(NFFT/2):fn)';
coherence = sinc(2*f*dmic/cs);
coherence = abs(coherence); %TODO: Should I really abs this?

M = floor(length(x)/(blocksize/2));
phi_nn = zeros(NPSD, 1); %TODO: better initial condition?
s_hat = zeros(N, 1);
s_hatpf = zeros(N, 1);
H = zeros(NPSD, 1);
if mod(blocksize,2) == 0
    w = hann(blocksize, 'periodic');
else
    w = hann(blocksize, 'symmetric');
end
tic
%TODO: The 50% overlap do not exactly add to 1, fix this if necessary.
%TODO: smooth over frequency above f0 according to [9]
E_after = [];
E_init = [];
db_drop = [];
%[yhat, hhat, e] = nlms(y(1:blocksize/2), x(1:blocksize/2), filtersize, 0.15);
%x1_old = x(1:blocksize/2);
%x2_old = filter(hhat, 1, y(1:blocksize/2));
%phiDiff = pwelch(x(1:4), [], [], NFFT) - pwelch(y(1:blocksize/2), [], [], NFFT);

meanDiff = [];
%figure(1)
phi_pld_old = zeros(NPSD, 1);
diffdiff = [];
diffdiff_ne = [];
phiDiff = zeros(NPSD, 1);
pldne_diff = [];
phi_pldne_old = zeros(NPSD, 1);
%TODO: Everything should be normalized to order of 1
for m=1:M-1
    t = 1+(m-1)*blocksize/2:(m+1)*blocksize/2;
    %t_last = 1+m*blocksize/2:(m+1)*blocksize/2;

    %TODO: How should the signals be normalized with respect to eachother?
    x1 = x(t).*w;
    x2 = y(t).*w;

    phi1_raw = pwelch(x1, [], [], NFFT); %TODO: Calculated over recursive time ALPHA1 and frame overlap and window etc. etc.
    phi1 = phi1_raw - phiDiff; %TODO: Should it be phi - phiDiff or just phi, seems ot make it better
    phi2 = pwelch(x2, [], [], NFFT);
    
    phi_12 = cpsd(x1,x2, [], [], NFFT); %Cannot have cpsd on x1 and x2
    phi_12 = abs(phi_12); %TODO: Should I really abs this? -> yeah
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %%%% Potentially a good vad %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    phi_pld = phi1-phi2;
    phi_pld(phi_pld<0) = 0;
    scalefactor = mean(abs(phi_pld))/( mean(abs(phi_pld_old)) + eps);
    tmp = phi_pld - phi_pld_old*scalefactor;

    norm_tot = sqrt(phi_12);
    norm_tot(norm_tot<eps) = eps;
    %normfactor = 1/sum(phi_12)*sum(phi1_raw)/sum(phi2);
    %normfactor = sum(phi1_raw.*phi2./phi_12);
    diffdiff = [diffdiff; sum(abs(tmp))];
    diffdiff_ne = [diffdiff_ne; mean(abs(tmp))/NORM];
    
    phi_pld_old = phi_pld;
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    phi1(phi1<0) = 0;
    phi_pld = phi1-phi2;
    %phi_pld = abs(phi_pld);
    phi_pld(phi_pld<0) = 0; %TODO: Is this better than abs()-ing it?
    phi_pldne = phi_pld./(phi1+phi2+eps);
    pldne_diff = [pldne_diff; mean(abs(phi_pldne-phi_pldne_old))];
    meanDiff = [meanDiff; mean(phi_pldne)];
    phi_pldne_old = phi_pldne;
    %TODO: Adaptation of phi_diff could potentially be in loop below,-> but
    % then it is to late?
    if diffdiff(end) < 2*10^-3
        phiDiff = (1-LAMBDA)*phiDiff + LAMBDA*(phi1-phi2);
        phi1 = phi1_raw - phiDiff;
        phi_pld = phi1-phi2;
        phi_pld(phi_pld<0) = 0;
        phi_pldne = phi_pld./(phi1+phi2+eps);
    end
 
    %Estimating phi_nn
    X1 = fft(x1, NFFT);
    X2 = fft(x2, NFFT);
    X1_mag = abs(X1).^2/NFFT; %Is this the best scaling?
    X2_mag = abs(X2).^2/NFFT;
    for n=1:NPSD
        if phi_pldne(n) < PHI_MIN %No speech
            phi_nn(n) = ALPHA2*phi_nn(n) + (1-ALPHA2)*X1_mag(end+1-n);%TODO: from where should I calculate X_mag?
            %H(n) = (phi_12(n) - phi_nn(n)*coherence(n))./(phi1(n)-phi_nn(n)+eps);
            H(n) = 0; %TODO:  not according to article. Is this good?
            %phiDiff(n) = phi1(n)-phi2(n);
        elseif phi_pldne(n) > PHI_MAX %speech
            phi_nn(n) = phi_nn(n); %Keep old value
            H(n) = (phi_12(n) - phi_nn(n)*coherence(n))./(phi1(n)-phi_nn(n)+eps);
            %phiDiff(n) = phiDiff(n);
        else %sorta no speech
            phi_nn(n) = ALPHA3*phi_nn(n) + (1-ALPHA3)*X2_mag(end+1-n);
            H(n) = (phi_12(n) - phi_nn(n)*coherence(n))./(phi1(n)-phi_nn(n)+eps);
        end
    end
  
    %Calculate gain filter G
    H_mag = abs(H).^2;
    %when the denominator in equation 18 is zero probably the numerator 
    %should be zero aswell. But because of bad coherence function it is not and this filter blows up
    H_mag(H_mag>1) = 0; %Easy fix
    H_mag = zeros(NPSD, 1); %TODO: This is better than how H was defined in rev52 and earlier, but can it be better?
    
    G = phi_pld./(phi_pld + GAMMA*(1-H_mag).*phi_nn+eps);
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
    %figure(11); plot(t, x(t), 'r'); hold on; plot(t, real(s_hat(t)), 'b');
    E_after(m) = mean(abs(s_hat(t)).^2);
    E_init(m) = mean(abs(x(t)).^2);
    db_drop(m) = 20*log10(E_after(m)/E_init(m));
end
disp(toc/(M-1));

%E_init = sum(x.^2)
%E_after = sum(real(s_hat).^2)
diffdiff_ne(diffdiff_ne>0.5) = 1;
diffdiff_ne(diffdiff_ne<0.5) = 0;
figure; plot(db_drop);
figure; plot((1:N)/N, x, 'r'); hold on;
           plot((0:(N-1))/N, s_hat, 'b');
           plot((0:(N-1))/N, s_hatpf, 'k'); 
           %plot((1:(M-1))/(M-1), diffdiff, 'g');
           plot((0:(M-2))/(M-1), diffdiff_ne, 'm');
           %plot((1:(M-1))/(M-1), pldne_diff, 'c');
           %plot((1:(M-1))/(M-1), meanDiff, 'c');
           yL = get(gca,'YLim');
           for X=0:1/(M-1):(M-2)/(M-1)
               line([X,X], yL, 'Color', 'b', 'LineStyle', ':')
           end
           hold off;
            
%sound(s_hat, fs)
%Hf = fdesign.lowpass('N,Fc',100,0.4);
%Hd1 = design(Hf,'window','window',@hamming,'SystemObject',true);
%filter(Hd1.coeffs.Numerator, 1, s_hat)
  