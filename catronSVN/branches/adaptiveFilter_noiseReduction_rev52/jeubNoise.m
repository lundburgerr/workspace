%function s = jeubNoise(x,y)
%s = jeubNoise(x,y)
%s is the noise reduced signal.
%x is headset mic (closest to mouth).
%y is the mic collecting noise.

%TODO: Test frame overlap as in the paper (with hann window) (Is this only
%for calculating the psd?


%Get signal
[s, fs] = wavread('2013-05-31 - noiseAndTalk_noMovement.wav');
fn = fs/2;
x = s(50000:end,2); %speech + noise
y = s(50000:end,1); %noise
blocksize = 320;
NFFT = 319; %this should be possible to set arbitrarily
filtersize = 40;
if mod(NFFT,2) == 0
    NPSD = NFFT/2+1;
else
    NPSD = (NFFT+1)/2;
end

scalefactor = sum(x(1:5000))/sum(y(1:5000));
x = scalefactor*x;
%frame_overlap = 0.5;

%Need inverse filter between the mics, otherwise the PSD will not take into
%consideration the physical differences between the mics
x=x(1001:end);
y=y(1001:end);
N = length(x);

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

%Estimated coherence
dmic = 0.4; %distance in meter between mics in average
cs = 343.6; %speed of sound
f = (0:fn/(NFFT/2):fn)';
coherence = sinc(2*f*dmic/cs);
coherence = abs(coherence); %TODO: Should I really abs this?


%The algorithm
M = floor(length(x)/(blocksize/2));
phi_nn = zeros(NPSD, 1); %TODO: better initial condition?
s_hat = zeros(N, 1);
H = zeros(NPSD, 1);
w = hann(blocksize);
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
phiDiff = pwelch(x(1:4000), [], [], NFFT) - pwelch(y(1:4000), [], [], NFFT);
meanDiff = [];
for m=1:M-1
    t = 1+(m-1)*blocksize/2:(m+1)*blocksize/2;
    t_last = 1+m*blocksize/2:(m+1)*blocksize/2;
    %Adapting a filter used when comparing the PSDs
%     
%     x1_new = x(t_last);
%     x2_new = y(t_last);
%     %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%     %%This is an ugly fix, insert VAD here
%     phi1 = pwelch(x1_new, [], [], NFFT); %TODO: Calculated over recursive time ALPHA1 and frame overlap and window etc. etc.
%     phi2 = pwelch(filter(hhat,1,x2_new), [], [], NFFT);
%     phi_pld = phi1-phi2; %TODO: Why is always x2 so much smaller after filtering than x1?
%     phi_pld(phi_pld<0) = 0;
%     phi_pldne = abs(phi_pld./(phi1+phi2));
%     mean(phi_pldne)
%     if mean(phi_pldne) < PHI_MIN
%     %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%         [yhat, hhat, e] = nlms(x2_new, x1_new, 40, 0.15, hhat); %TODO: Don't use new filter if speech in mic
%     end
%     x2_new = filter(hhat, 1, x2_new);
%     x1 = [x1_old; x1_new].*w;
%     x2 = [x2_old; x2_new].*w;
%     x1_old = x1_new;
%     x2_old = x2_new;
    %TODO: How should the signals be normalized with respect to eachother?
    
    x1 = x(t);
    x2 = y(t);
    
    X1 = fft(x1);
    X1_mag = abs(X1).^2/length(X1); %TODO: Need to normalize this somehow
    X2_mag = abs(fft(x2)).^2/length(x2); %TODO: Need to normalize this somehow
    phi1 = pwelch(x1, [], [], NFFT); %TODO: Calculated over recursive time ALPHA1 and frame overlap and window etc. etc.
    phi1 = phi1 - phiDiff; %Take into consideration that they are not equal mics
                            %TODO: Should it be - phiDiff
    phi2 = pwelch(x2, [], [], NFFT);
    phi_12 = cpsd(x1,x2, [], [], NFFT);
    phi_12 = abs(phi_12); %TODO: Should I really abs this?
    
    phi_pld = phi1-phi2;  %Take into consideration that the mics are not equal
                          %TODO: Maybe I always need to make sure
                          %they are of same strength and adapt the
                          %scalefactor every block
    phi_pld(phi_pld<0) = 0; %Everything less than zero is zero
    
    phi_pldne = abs(phi_pld)./(phi1+phi2); %Calculate this not using the phi_pld
     meanDiff = [meanDiff;mean(phi_pldne)];
%     if mean(phi_pldne) < 0.01
%         phiDiff = phi1-phi2;
%     end
    
    %Estimating phi_nn
    %TODO: This could potentially be speed up by assuming that phi_pldne is same
    %for all samples in block
    for n=1:length(phi_pldne)
        if phi_pldne(n) < PHI_MIN %No speech
            phi_nn(n) = ALPHA2*phi_nn(n) + (1-ALPHA2)*X1_mag(end+1-n);%TODO: from where should I calculate X_mag?
            H(n) = 0; %TODO:  not according to article. Is this good?
            phiDiff(n) = phi1(n)-phi2(n);
        elseif phi_pldne(n) > PHI_MAX %speech
            phi_nn(n) = phi_nn(n); %Keep old value
            H(n) = (phi_12(n) - phi_nn(n)*coherence(n))./(phi1(n)-phi_nn(n)+eps);
            phiDiff(n) = phiDiff(n);
        else %sorta no speech
            phi_nn(n) = ALPHA3*phi_nn(n) + (1-ALPHA3)*X2_mag(end+1-n);
            H(n) = (phi_12(n) - phi_nn(n)*coherence(n))./(phi1(n)-phi_nn(n)+eps);
        end
    end
  
    %Calculate gain filter G
    %phi_n1n2 = estimated how?
    %H = (phi_12 - phi_n1n2)./(phi1-phi_nn);
    %H = (phi_12 - phi_nn.*coherence)./(phi1-phi_nn+eps);
    H_mag = abs(H).^2;
    
    %when the denominator in equation 18 is zero probably the numerator 
    %should be zero aswell. But because of bad coherence function it is not and this filter blows up
    %H_mag(H_mag>2) = 0;
    %H_mag(H_mag>1) = 1;
    H_mag(H_mag>1) = 0; %Easy fix
    G = phi_pld./(phi_pld + GAMMA*(1-H_mag).*phi_nn+eps);
    G = resample(G, blocksize/2, length(G));
    
    %G = resample(G, blocksize, length(G));
    %G = G(1:blocksize/2);
    %G = [G(1); G(1:end-1); flipud(G)]; %TODO: this should be made smoother losing one value here
                                    %TODO: does it work for general blocksize?
    %s_hat(1+(m-1)*blocksize:m*blocksize) = ifft(abs(G).*X1);
    %t_half = 1+(m-1)*blocksize/2:m*blocksize/2;
    %s_tmp = ifft(G.*X1);
    s_hat_feud = ifft([G(1); G(1:end-1); flipud(G)].*X1); %which one is right?
    
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    %%Smooth higher frequencies above f0 according to [9] seen from jeub
    zeta = sum(abs(s_hat_feud).^2)/sum(abs(x(t_last).*w(1+blocksize/2:end)).^2); %TODO: Pretty ugly with this halfwindow
    zetaT = 1;
    if zeta<ZETAthr
        zetaT = zeta;
    end
    
    T = 1; %N in [9]
    if zetaT ~= 1
        T = 2*round((1-zetaT/ZETAthr)*PSI_SCALE)+1;
    end
          
    K = ones(blocksize/4, 1); %filter, H in [9]
    for n = 1:length(K)
        if n < T
            K(n) = 1/T; %in dB it is 1/T lower
        else
            K(n) = 0;
        end
    end
    K = 10.^(-K/20);
    %K = resample(K, blocksize/2, length(K));
    %K = [K(1); K(1:end-1); flipud(K)]; %TODO: this should be made smoother losing one value here
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
%     Gpf = filter(K, 1, abs(G));
%     Gpf = resample(Gpf, blocksize/2, length(Gpf));
%     Gpf = [Gpf(1); Gpf(1:end-1); flipud(Gpf)];
    G = [G(1); G(1:end-1); flipud(G)];
%     n_start = round(length(G)*F0/fn); %This can be calculated outside loop
%     Gtot = [G(1:n_start); Gpf(n_start+1:end)];
%     Gtot = [Gtot(1); Gtot(1:end-1); flipud(Gtot)]; %TODO: this should be made smoother losing one value here
    s_hat(t) = s_hat(t) + ifft(G.*X1);
    %plot(t, x(t), 'r'); hold on; plot(t, real(s_hat(t)), 'b');
    E_after(m) = mean(abs(s_hat(t)).^2);
    E_init(m) = mean(abs(x(t)).^2);
    db_drop(m) = 20*log10(E_after(m)/E_init(m));
end
toc/M

%E_init = sum(x.^2)
%E_after = sum(real(s_hat).^2)
figure(1); plot(db_drop);
figure(2); plot(x, 'r'); hold on; plot(real(s_hat), 'b');
figure(3); plot(meanDiff)
%sound(s_hat, fs)
%Hf = fdesign.lowpass('N,Fc',100,0.4);
%Hd1 = design(Hf,'window','window',@hamming,'SystemObject',true);
%filter(Hd1.coeffs.Numerator, 1, s_hat)
    