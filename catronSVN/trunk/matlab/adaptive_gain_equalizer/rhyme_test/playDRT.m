function playDRT(fileName, fs)
% Function to run DRT tests headsetMic_mos_drt_robin_120dBA_zon3_denoisedNoVad
%
% Syntax: playDRT(fileName, fs)
%
% Inputs:
%    fileName - name of a .dat file without file ending (ex:
%               'drt_words_zone2_120dB'), located under \Rhyme_recordings
%    fs - sampling frequency
%
% Outputs:
%    None - Play GUI
%
% Example: 
%
% Other m-files required: none
% Subfunctions: none
% MAT-files required: fileName.MAT - splitpoint intervalls of the words
%                     saved under the same filname as the .dat file.
%                     Located under \Rhyme_recordings\splitpoints
%                     The saved variable in the .dat file must be named
%                     files_size
%
% txt-files required: list of words in the .dat file. Located under
%                     \Rhyme_recordings\words
%
slCharacterEncoding('ISO-8859-1')

% path of recordings
path = strcat(pwd, '\Rhyme_recordings\');

% import .dat file with recored words
sig = importdata([path,fileName,'.dat'], ' ', 1);
s = sig.data;

% import the splitpoints
fileSplita = strcat('splitpoints\',fileName,'.MAT');

load(strcat(path, fileSplita));
splita = files_size;

%Read in words
n_words_to_compare = 2;
n_pairs = 52;

remove_words = 0;

if remove_words
    pathWords = strcat(pwd,'\Rhyme_recordings\words\drt_words_temp.txt');
else
    pathWords = strcat(pwd,'\Rhyme_recordings\words\drt_words.txt');
end
f = fopen(pathWords);

words = cell(n_pairs, n_words_to_compare);
for k=1:n_pairs
    for j=1:n_words_to_compare
        words{k,j} =  fscanf(f, '%s', 1);
    end
end
fclose(f);

%Split signal into words, each group has 2 words
S = cell(length(splita)/n_words_to_compare, n_words_to_compare);
index = 1;
for k = 1:length(splita)/n_words_to_compare
    for j=1:n_words_to_compare
        S{k,j} = s(index:index+splita((k-1)*n_words_to_compare+j)-1);
        index = index+splita((k-1)*n_words_to_compare+j);
    end
end

%% Ta bort orden med å ä ö
if remove_words
    S_temp = cell((length(splita)/n_words_to_compare)-6, n_words_to_compare);
    
    ind=1;
    for i=1:(length(splita)/n_words_to_compare)-6
        for j=1:2
            if ind == 1 || ind == 3 || ind == 13
                ind=ind+1;
            end
            S_temp{i,j} = S{ind,j};
        end
        ind = ind+1;
    end
   
    words{1,1} = 'mat';
    
    %Run rhymetest
    rhymetest(S_temp, words, fs);
else
    words{1,1} = 'båt';
    words{1,2} = 'bår';
    words{3,1} = 'båda';
    words{3,2} = 'låda';
    words{13,1} = 'grön';
    words{13,2} = 'gröt';
    
    %Run rhymetest
    rhymetest(S, words, fs);
end