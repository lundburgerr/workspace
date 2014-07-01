function h = velfind(laser1, laser2, fs)

thresh = 0.03;
extrema = -1; %Search for minima
include_endpoints = false;

laser1 = smooth(laser1, 25);
laser2 = smooth(laser2, 25);

%% Find peaks
sel = (max(laser1)-min(laser1))/4; %TODO: What is this?
peakLoc1 = peakfinder(laser1, sel, thresh, extrema, include_endpoints);

sel = (max(laser2)-min(laser2))/4;
peakLoc2 = peakfinder(laser2, sel, thresh, extrema, include_endpoints);


%TODO: Check this
%Make sure the first peaks correlate to eachother, otherwise shift one
%step.
if abs(peakLoc1(1)-peakLoc2(1)) > abs(peakLoc1(1)-peakLoc2(2))
    peakLoc2 = peakLoc2(2:end);
end
if abs(peakLoc2(1)-peakLoc1(1)) > abs(peakLoc2(1)-peakLoc1(2))
    peakLoc1 = peakLoc1(2:end);
end

%Calculate length and make sure they're of equal size
N = min(length(peakLoc1), length(peakLoc2));
peakLoc1 = peakLoc1(1:N);
peakLoc2 = peakLoc2(1:N);

%Find peaks that are to close and average them out if needed
% sthresh = 100;
% for ii=1:N-1
%     sDiff = peakLoc1(ii+1)-peakLoc1(ii);
%     if(sDiff < sthresh)
%         peakLoc1(ii) = (peakLoc1(ii+1)+peakLoc1(ii))/2;
%         peakLoc1 = [peakLoc1(1:ii); peakLoc1(ii+2:end)];
%         ii..
%     end
% end
shift = 10;
change = 10;
for ii=1:N
    %Laser 1
    curmin = mean(laser1(peakLoc1(ii)-shift:peakLoc1(ii)+shift));
    newmin = mean(laser1(peakLoc1(ii)-2*shift:peakLoc1(ii)));
    while  curmin > newmin
        peakLoc1(ii) = peakLoc1(ii)-change;
        curmin = mean(laser1(peakLoc1(ii)-shift:peakLoc1(ii)+shift));
        newmin = mean(laser1(peakLoc1(ii)-2*shift:peakLoc1(ii)));
    end
    curmin = mean(laser1(peakLoc1(ii)-shift:peakLoc1(ii)+shift));
    newmin = mean(laser1(peakLoc1(ii):peakLoc1(ii)+2*shift));
    while  curmin > newmin
        peakLoc1(ii) = peakLoc1(ii)+change;
        curmin = mean(laser1(peakLoc1(ii)-shift:peakLoc1(ii)+shift));
        newmin = mean(laser1(peakLoc1(ii):peakLoc1(ii)+2*shift));
    end
    
    %Laser 2
    curmin = mean(laser2(peakLoc2(ii)-shift:peakLoc2(ii)+shift));
    newmin = mean(laser2(peakLoc2(ii)-2*shift:peakLoc2(ii)));
    while  curmin > newmin
        peakLoc2(ii) = peakLoc2(ii)-change;
        curmin = mean(laser2(peakLoc2(ii)-shift:peakLoc2(ii)+shift));
        newmin = mean(laser2(peakLoc2(ii)-2*shift:peakLoc2(ii)));
    end
    curmin = mean(laser2(peakLoc2(ii)-shift:peakLoc2(ii)+shift));
    newmin = mean(laser2(peakLoc2(ii):peakLoc2(ii)+2*shift));
    while  curmin > newmin
        peakLoc2(ii) = peakLoc2(ii)+change;
        curmin = mean(laser2(peakLoc2(ii)-shift:peakLoc2(ii)+shift));
        newmin = mean(laser2(peakLoc2(ii):peakLoc2(ii)+2*shift));
    end
end

%Calculate sample difference
tDiff = abs(peakLoc1-peakLoc2)/fs;
Enorm = (1./tDiff).^2;
Enorm = Enorm/Enorm(1);
time = abs(peakLoc1+peakLoc2)/(2*fs);

%Fit data to exponential decay
%fitType = 'exp1';
fitType = @(a,x) exp(-a*x);
myfit = fit(time, Enorm, fitType, 'StartPoint', 0.1);
ci = confint(myfit);

%% Fit parameters to model using least squares
h = figure;
plot(time, Enorm, time, myfit(time), '--r');
xlabel('Time (s)', 'FontSize', 18); ylabel('Energy (normalized)', 'FontSize', 18);
legend('Measured energy', sprintf('e^{-%ft}', myfit.a));



