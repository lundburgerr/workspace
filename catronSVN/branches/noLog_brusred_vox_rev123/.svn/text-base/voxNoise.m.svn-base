function [db_drop, meanDiff, Gpfnrj] = voxNoise(x, y)
%s_hat = PSDdiff_jeubNoise(x, y)
%s_hat      - the noise reduced signal
%
%x          - headset mic closest to usable signal source
%y          - mic collecting noise.
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
% Created - 2013-Jun-27
% Last modification - 2013-Jun-27

global vadval;

global blocksize; %has to be even
global NFFT;
global NPSD;
global fs;

global phiNN;
global phiXX;
global phiYY;
global phiXXsmooth;
global phiYYsmooth;
global P;
global Psmooth;
global scaling;
global Gain;
global w;

global s_hat;

global scalingPlot;
global PLDPlot;
global specGainPlot;

fn = fs/2;

%Parameter values, constants
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
PHI_MIN = 0.2;
PHI_MAX = 0.8;
GAMMA = 6;
ALPHA1 = 0.9; %TODO: what is this used for?
ALPHA2 = 0.9;
ALPHA3 = 0.8;

ZETAthr = 0.4; %Smoothing threshold for PF filter [9]
PSI = 10; %TODO: scaling factor for PF filter
F0 = 1000; %smooth out after F0Hz

LAMBDA = 0.05; %Adaptation speed for Px,Py
GAINMIN = 10^(-12/20);

FALL = 0.85;
RISE = 3;

SIZEADAP = 0.15; %maximum adaptation -1 of size
MALENGTH = 9; %Probably should be an odd integer
SCALEADAPT = 0.001;

%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%


%%%%The algorithm
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
x1 = x.*w;
x2 = y.*w;%*scaling;

%TODO: Convert to fixed points
X1 = fft(x1, NFFT);
X2 = fft(x2, NFFT);
X1mag = abs(X1).^2./NFFT; %TODO: DO I need to scale this?Is this the best scaling?
X2mag = abs(X2).^2./NFFT; %% WHY 0.1 ???
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

X1magsmooth = filter(ones(MALENGTH,1)/MALENGTH, 1, X1mag);
X2magsmooth = filter(ones(MALENGTH,1)/MALENGTH, 1, X2mag);


X1mag_pre = X1mag(1:NPSD);
X2mag_pre = X2mag(1:NPSD);
X1magsmooth_pre = X1magsmooth(1:NPSD);
X2magsmooth_pre = X2magsmooth(1:NPSD);

X1mag = X1mag(1:NPSD);
X1magsmooth = X1magsmooth(1:NPSD);
X2mag = X2mag(1:NPSD).*(10.^(-P./10));
X2magsmooth = X2magsmooth(1:NPSD).*(10.^(-Psmooth./10));



phiXX = (1-ALPHA1)*phiXX + ALPHA1*X1mag;%TODO: Should it be phi - phiDiff or just phi, seems ot make it better
phiYY = (1-ALPHA1)*phiYY + ALPHA1*X2mag;
phiXXsmooth = (1-ALPHA1)*phiXXsmooth + ALPHA1*X1magsmooth;%TODO: Should it be phi - phiDiff or just phi, seems ot make it better
phiYYsmooth = (1-ALPHA1)*phiYYsmooth + ALPHA1*X2magsmooth;

phiPLDsmooth = phiXXsmooth-phiYYsmooth;
phiPLDsmooth(phiPLDsmooth<0) = 0;

phiPLD = phiXX-phiYY;
phiPLD(phiPLD<0) = 0;

phiPLDne = phiPLDsmooth./(phiXXsmooth+phiYYsmooth+eps);

%%%%

specGainPlot = [specGainPlot, mean(Psmooth)];

%%%%

%PLDPlot = [PLDPlot, mean(phiPLDne)];
%%%%

[lambda, p, speech, voxScale] = vox(phiXXsmooth, phiNN);


%for n=1:NPSD
 %   if phiPLDne(n) < PHI_MIN %&& phiPLDne(n) > -PHI_MIN %No speech
  %      phiNN(n) = ALPHA2*phiNN(n) + (1-ALPHA2)*X1magsmooth(n);
   % elseif phiPLDne(n) < PHI_MAX
    %    phiNN(n) = ALPHA3*phiNN(n) + (1-ALPHA3)*X2magsmooth(n);
    %end
%end

for n=1:NPSD
    if phiPLDne(n) < PHI_MIN
 %  if lambda(n) < 20
       phiNN(n) = ALPHA2*phiNN(n) + (1-ALPHA2)*X1magsmooth(n);
    elseif phiPLDne(n) < PHI_MAX
        phiNN(n) = ALPHA3*phiNN(n) + (1-ALPHA3)*X2magsmooth(n);
    end
end


PLDPlot = [PLDPlot, voxScale];
if ~speech
    %TODO: adapt according to likelihood
    logP = 10*log10(X2mag_pre)-10*log10(X1mag_pre);
    
    postDiff = 10*log10(X2magsmooth)-10*log10(phiNN);
    a = mean(phiNN);
    scalingPlot = [scalingPlot, 10*log10(500000*mean(phiNN))];
    
    P = (1-LAMBDA)*P + LAMBDA*logP;
   % P(P>10) = 10;
    
    logPsmooth = 10*log10(X2magsmooth_pre)-10*log10(X1magsmooth_pre);
    Psmooth = (1-LAMBDA)*Psmooth + LAMBDA*logPsmooth;
   %Psmooth(Psmooth>10) = 10;
    

  %  scale = 10*log((sum(abs(X2))/sum(abs(X1))));
  %  if scale > 1+SIZEADAP
  %      scale = 1+SIZEADAP;
  %  elseif scale< 1-SIZEADAP
  %      scale = 1-SIZEADAP;
  %  end
    %TODO: adapt according to likelihood
  %  scaling = (1-SCALEADAPT)*scale*scaling + SCALEADAPT*scaling;
    %scaling = (1-p)*scaling*scale + p*scaling;
end

%Calculate gain filter
G = phiPLD./(phiPLD + GAMMA*phiNN+eps);
for n=1:NPSD
    if Gain(n) > G(n)
        Gain(n) = Gain(n)*FALL;
        Gain(n) = max(G(n), Gain(n));
    else
        Gain(n) = Gain(n)*RISE;
        Gain(n) = min(G(n), Gain(n));
    end
end
Gain(Gain<GAINMIN) = GAINMIN;
Gain = musicSupp(Gain, X1(1:NPSD), ZETAthr, PSI, round(F0/fn*NPSD));

%if speech
    Gainsym = [Gain(1:end-1); flipud(Gain(2:end))];
    stmppf = ifft(Gainsym.*X1) .* voxScale ;
%else
%    stmppf = zeros(blocksize,1);
%end
s_hat = [s_hat(blocksize/2+1:end); zeros(blocksize/2,1)];
s_hat = s_hat + stmppf(1:blocksize);

vadval = [vadval; p];

  
  