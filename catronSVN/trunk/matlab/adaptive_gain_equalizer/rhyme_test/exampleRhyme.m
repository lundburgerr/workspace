%All the files needed can be located in \\local.limesaudio.com\lkolon\Limes Audio\Test-ljudfiler
%Extract rhyme-test.zip and change root path below.
%Also choose what noise file you want below or make your own

root = 'D:\rhyme-test\';
path = strcat(root, 'MRT Streams\');
filea = 'm2a.wav';
fileb = 'm2b.wav';
fileSplita = 'splitpoints\m2a.MAT';
fileSplitb = 'splitpoints\m2b.MAT';
load(strcat(path, fileSplita));
splita = files_size;
fileNamesa = in_files;

%Read in words
pathWords = strcat(root, 'words.txt');
f = fopen(pathWords);
words = cell(25, 6);
for k=1:25
    for j=1:6
        words{k,j} =  fscanf(f, '%s', 1);
    end
end
fclose(f);

pathnoise = strcat(root, 'bg_noise\bg_loops\');
%noise = 'short_burst.wav';
noise = 'taxing.wav';
%noise = 'chainsaw.wav';
%noise = 'firetruck_pump_panel.wav';
%noise = 'fog_nozzle.wav';
%noise = 'k12_saw.wav';
%noise = 'low_air_alarm.wav';
%noise = 'nightclubloop-16bit.wav';
%noise = 'PASS1.wav';
%noise = 'PASS2.wav';
[sn_short, fsnoise] = wavread(strcat(pathnoise, noise));
[s, fssig] = wavread(strcat(path, filea));

%Create noise the length of the signal file
%blocksize = floor(length(sn_short));
% if mod(blocksize,2) ~= 0
%     blocksize = blocksize-1;
% end
% w = hann(blocksize, 'periodic');
sn = zeros(length(s)+2*length(sn_short),  min(size(sn_short)));
m = 0;
while (m)*length(sn_short) < length(s)
    %t = 1+(m-1)*blocksize/2:(m+1)*blocksize/2;
    t = 1+m*length(sn_short):(m+1)*length(sn_short);
    sn(t,:) = sn_short;
    m = m+1;
end
sn = sn(1:length(s),:);

%Create signal and noise signal
fs = 8000;
sn = resample(sn, fs, fsnoise);
s = resample(s, fs, fssig);

% assuming that stereo signals consist of a voice and a noise channel 
if size(sn,2) == 2 
    a = 0.02*mean(s(:,1).^2)/mean(sn(:,1).^2);
    x = s + a*sn(:,1);
    y = a*sn(:,2);   
else
    x = s + sn/2;
    y = s/10 + sn/2;
end

splita = floor(splita*fs/fssig)+round(fssig/fs);

overshoot = sum(splita)-length(x);
while abs(overshoot) > length(splita)
    if overshoot > 0
        splita = splita - 1;
        overshoot = overshoot - length(splita);
    elseif overshoot < 0
        splita = splita + 1;
        overshoot = overshoot + length(splita);
    end
end
    
if overshoot > 0
    splita(end-overshoot+1:end) = splita(end-overshoot+1:end) - 1;
elseif overshoot < 0
    splita(end-overshoot+1:end) = splita(end-overshoot+1:end) + 1;
end

NFFT = 512;
blocksize = 320;

yphi = pwelch(y(1:312), [], [], 512);
Py = 20*log10(yphi)-mean(20*log10(yphi));
xphi = pwelch(x(1:312), [], [], 512);
Py(Py<-20)=-20;
Px = 20*log10(xphi)-mean(20*log10(xphi));
Px(Px<-20)=-20;
Px(:) = 0;
Py(:) = 0;

[s_hat, s_hatpf, db_drop, meanDiff] = jeubNoise(x, y, Px, Py, NFFT, blocksize, fs);
N = length(s);
M = floor(N/(blocksize/2));
%figure; plot(db_drop);
figure; plot((1:N)/N, s, 'r'); hold on;
           plot((0:(N-1))/N, s_hat, 'b');
           plot((0:(N-1))/N, s_hatpf, 'k'); 
           %plot((1:(M-1))/(M-1), diffdiff, 'g');
           plot((0:(M-2))/(M-1), meanDiff/10, 'g');
           hold off;

S = cell(length(splita)/6, 6);
index = 1;
for k = 1:length(splita)/6
    for j=1:6
        S{k,j} = s_hatpf(index:index+splita((k-1)*6+j)-1);
        index = index+splita((k-1)*6+j);
    end
end

% sn = resample(sn, fs, fsnoise);
% for k = 1:length(splita)
%     S{k} = resample(S{k}, fs, fssig);
% end

save('rhymeData.MAT', 'S', 'words', 'fs');
