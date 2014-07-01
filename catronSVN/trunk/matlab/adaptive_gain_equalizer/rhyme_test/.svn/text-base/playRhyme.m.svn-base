function playRhyme(s, fs)
%run  playRhyme(s(25000:end), fs);
root = 'D:\rhyme-test\';
path = strcat(root, 'MRT Streams\');
% filea = 'm2a.wav';
% fileb = 'm2b.wav';
fileSplita = 'splitpoints\m2a.MAT';
% fileSplitb = 'splitpoints\m2b.MAT';
load(strcat(path, fileSplita));
splita = files_size;
% fileNamesa = in_files;

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

% The split intervals are changed if the sampling frequency is different
% than??
fsRhyme = 48000;
splita = floor(splita*fs/fsRhyme)+round(fsRhyme/fs);

overshoot = sum(splita)-length(s);
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

%Split signal into words, each group has 6 words
S = cell(length(splita)/6, 6);
index = 1;
for k = 1:length(splita)/6
    for j=1:6
        S{k,j} = s(index:index+splita((k-1)*6+j)-1);
        index = index+splita((k-1)*6+j);
    end
end

%Save rhymeData for use in rhymetest
%save('rhymeData.MAT', 'S', 'words', 'fs');

%Run rhymetest
rhymetest(S, words, fs);