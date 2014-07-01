function [lambda, p, v, voxScale] = vox(Py, Pn)

Py(Py == 0) = 0.0001;
Pn(Pn == 0) = 0.0001;

alpha = 0.3;
p_vox_begin = 0.01;
p_vox_end = 0.1;

ss_p_h0 = p_vox_end / (p_vox_end + p_vox_begin);
ss_p_h1 = 1 - ss_p_h0;

p_start_vox = 0.1;

p_max = 5000;
p_thresh = 0.05;

persistent old_p;
persistent old_gamma;
persistent old_xi;
persistent old_v;
persistent vs;

if isempty(vs)
    vs = 0;
end


if isempty(old_p)
    old_p = p_start_vox;
end

if isempty(old_gamma)
    old_gamma = Py ./ Pn;
end

if isempty(old_xi)
    old_xi = Py ./ Pn;
    old_xi(old_xi == 0) = 0.00001;
end

if isempty(old_v)
    old_v = old_gamma.*old_xi./(1+old_xi);
end

gamma = Py ./ Pn; % A posteriori SNR

ex = exp(0.5*expint_sam(old_v));
old_gain = (old_xi ./ (1 + old_xi)) .* ex;

xi = alpha * old_gain.^2 .* old_gamma + (1 - alpha).*max(gamma - 1, 0); % A priori SNR

v = gamma.*xi./(1+xi);

v(v > 10) = 10;

lambda = (1 / 1 + xi).*exp(v);

old_gamma = gamma;
old_xi = xi;
old_v = v;

lambda_gm = exp(mean(log(lambda)));



p = lambda_gm .* (p_vox_begin + (1 - p_vox_end).*old_p) ./ ((1 - p_vox_begin) + p_vox_end .*old_p);

old_p = p;

p(p > p_max) = p_max;
p = p ./ p_max;

if(p > p_thresh)
    v = 1;
else
    v = 0;
end

vs = vs - 0.05*(1.4 - vs) - 0.008;

if v
    vs = 1.4;
end

if vs<0
    vs = 0;
end

voxScale = vs;
if(voxScale > 1)
    voxScale = 1;
    
end

