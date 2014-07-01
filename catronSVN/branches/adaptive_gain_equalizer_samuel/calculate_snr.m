function [ r ] = calculate_snr( S, N, fs )
addPath ./toolkit/functions/general/;

[sInd, sAct] = active_speech(S, 0.001, fs, 0.1);

% figure
% plot(S)
% hold on
% plot(sAct,'r')


powN = mean((N-mean(N)).^2); % Ska det vara (sInd) här??
powS = mean((S(sInd)-mean(S(sInd))).^2);

r = 10*log10(powS/powN);

end

