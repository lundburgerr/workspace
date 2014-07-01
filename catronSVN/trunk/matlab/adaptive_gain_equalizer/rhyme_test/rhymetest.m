function rhymetest(S, words, fs)
%rhymetest(S, words, fs)
%S      - is a cell array with dimension cell(#groups, #words) where #groups is
%       number of groups of words and #words is how many words there are in
%       each group.
%words  - a cell array with the same dimensions as the sound array S where
%       each element corresponds to the right word.
%fs     - the sample frequency for the sound.
%
%This program is based on the modified rhyme test.

% Author - Robin Lundberg
% E-mail - robin.lundberg@limesaudio.com
% Website - www.limesaudio.com
% Copyright - Limes Audio AB
% Created - 2013-Jun-19
% Last modification - 2013-Jun-19

slCharacterEncoding('ISO-8859-1')

clear global;
global Words;
global Sounds;
global Pos;
global Map;
global Wordgroups;
global Groupsize;
global Fs;

Sounds = S;
Words = words;
Map = ones(size(Sounds));
[row, col] = find(Map);
newInd = ceil(rand()*length(row));
Pos = [row(newInd), col(newInd)];
Map(row(newInd), col(newInd)) = 0;
Fs = fs;
[Wordgroups, Groupsize] = size(Sounds);

guiRhyme();
