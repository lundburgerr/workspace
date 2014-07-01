function SNR=calc_snr(s,n,fs)
%   Input
%   s - signal with speech
%   n - signal with noise

debug_plot = 1;

[sInd, sAct] = active_speech(s, 0.001, fs, 0.01);

% Calulate SNR
P_x_signal = mean((s(sInd)).^2);
P_x_noise = mean(n.^2);

SNR = P_x_signal/P_x_noise;

if debug_plot
    figure;
    hold on;
    plot(s);
    plot(sAct,'r')
end

end