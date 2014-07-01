% Simple example on how to use PSDdiff_jeubNoise.m
% Author - Robin Lundberg
% E-mail - robin.lundberg@limesaudio.com


% hhat =  [-0.0113, -0.1845,0.2289,-0.0682,-0.0176,0.0367,-0.0278,0.0384,-0.0163,-0.0004,0.0074...
%     ,0.0113,-0.0075,0.0170,-0.0132,0.0155,0.0268,-0.0388,0.0257,-0.0120,-0.0361,0.0521,-0.0320...
%     ,0.0207,-0.0002,-0.0019,0.0292,-0.0186,0.0297,-0.0360,0.0031,-0.0056,-0.0001,0.0285...
%    ,-0.0213,0.0024,-0.0004,0.0055,-0.0051,-0.0077];
% [s, fs] = wavread('2013-05-31 - noiseAndTalk_noMovement.wav');
% x = s(:,2); %speech + noise
% y = s(:,1); %noise
% filtersize = 40;
% [yhat, hhat, e] = nlms(y(1:5000), x(1:5000), filtersize, 0.15);
addpath('../noiseReduction');

NFFT=512;
blocksize = 320;
%Get PSD subtract function
[s, fs] = wavread('2013-05-31 - noiseAndTalk_noMovement.wav');
xp = s(:,2); %speech + noise
yp = s(:,1); %noise
%[yhat, hhat, e] = nlms(yp(1:5000), xp(1:5000), filtersize, 0.15);

yphi = pwelch(yp(1:312), [], [], NFFT);
Py = 20*log10(yphi)-mean(20*log10(yphi));
xphi = pwelch(xp(1:312), [], [], NFFT);
Py(Py<-20)=-20;
Px = 20*log10(xphi)-mean(20*log10(xphi));
Px(Px<-20)=-20;

%TODO: Using these initial conditions, Px, Py never adapts, fix this!
% Px(:) = -100;
% Py(:) = 100;

%Get signal
if false
    [s, fs] = wavread('2013-05-31 - noiseAndTalk_noMovement.wav');
    x = s(:,2); %speech + noise
    y = s(:,1); %noise
    
%     [yhat, hhat, e] = nlms(y(1:5000), x(1:5000), filtersize, 0.15);
%     y = filter(hhat, 1, y);
%     
%     scalefactor = mean(abs(x(1:5000)))/mean(abs(y(1:5000)));
%     y = scalefactor*y;
% 
%     x=x(60000:end);
%     y=y(60000:end);
  
elseif false %TODO:Test this signal aswell
    %[s, fs_file] = wavread('Mic2_plan3_rörelse.wav', [5.61, 6]*10^6);
    [s, fs_file] = wavread('Mic2_plan3_rörelse.wav', [4.2, 4.6]*10^6);
    fs = 8000;
    s = resample(s, fs, fs_file);
    x = s(:,1); %speech + noise
    y = s(:,2); %noise
elseif true %TODO:Test this signal aswell
    %[s, fs_file] = wavread('startMotorPrat.wav');
    %[s, fs_file] = wavread('test0snr.wav');
    [s, fs_file] = wavread('test18snr.wav');
    fs = 8000;
    s = resample(s, fs, fs_file);
    x = s(:,1); %speech + noise
    y = s(:,2); %noise
elseif true
    %noise = 'short_burst.wav';
    root = 'D:\rhyme-test\';
    pathnoise = strcat(root, 'bg_noise\bg_loops\');
    noise = 'taxing.wav';
    filea = 'm2a.wav';
    [sn_short, fsnoise] = wavread(strcat(pathnoise, noise));
    [s, fssig] = wavread(strcat(path, filea));
    
    fs = 8000;
    s = resample(s, fs, fssig);
    sn_short = resample(sn_short, fs, fsnoise);
    s = s(1:length(sn_short));
    
    scale = 30;
    x = s+scale*sn_short(:,1)/3;
    y = scale*sn_short(:,2);
    
    x = x(5*fs:16*fs);
    y = y(5*fs:16*fs);
    s = s(5*fs:16*fs);
end


[s_hat, s_hatpf, db_drop, meanDiff, Gpfnrj] = jeubNoise(x, y, Px, Py, 1, NFFT, blocksize, fs);

N = length(x);
M = floor(N/(blocksize/2));
figure; plot(db_drop);
legend('dB drop');
xlabel('block');
ylabel('dB');
figure; plot((1:N)/N, x, 'r'); hold on;
        %plot((1:N)/N, s, 'b'); hold on;
           %plot((0:(N-1))/N, s_hat, 'b');
           plot((0:(N-1))/N, s_hatpf, 'k'); 
           %plot((1:(M-1))/(M-1), diffdiff, 'g');
           %plot((0:(M-2))/(M-1), meanDiff/10, 'g');
           %plot((0:(M-2))/(M-1), Gpfnrj/10, 'g');
           hold off;
legend('Original', 'Fixed', 'diff PSD');
           %sound(s_hatpf*10, fs)
wavwrite(x*10, fs, 'test_recordingOriginal.wav');
wavwrite(s_hatpf*10, fs, 'test_recordingFixed.wav');