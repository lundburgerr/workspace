function Gpf = musicSupp(G, Xmag, thresh, scalingfactor, smoothThresh)
%G goes from 0 to fn

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%Smooth higher frequencies above f0 according to [9] seen from jeub
[r, c] = size(G);
if r>1 && c>1
    throw('G needs to be a 1-dim vector');
end
if r == 1
    G = G';
end
N = length(G);
G = abs(G);
zeta = sum(abs(G.*Xmag).^2)/sum(abs(Xmag).^2);
zetaT = 1;
if zeta<thresh
    zetaT = zeta;
end

T = 1; %N in [9]
if zetaT ~= 1
    T = 2*round((1-zetaT/thresh)*scalingfactor)+1;
end

K = ones(N, 1); %filter, H in [9]
for n = 1:N
    if n < T+1
        K(n) = 1/T; %in dB it is 1/T lower
    else
        K(n) = 0;
    end
end

%K = 10.^(-K/20);
%K = resample(K, blocksize/2, length(K));
%K = [K(1); K(1:end-1); flipud(K)]; %TODO: this should be made smoother losing one value here
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Gpf = filter(K, 1, G);
Gpf = [G(1:smoothThresh); Gpf(smoothThresh+1:end)];
%Gpf = resample(Gpf, blocksize/2, length(Gpf));
%Gpf = [Gpf(1); Gpf(1:end-1); flipud(Gpf)];