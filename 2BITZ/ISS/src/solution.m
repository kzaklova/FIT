iptsetpref('UseIPPL',false);
S0 = imread('xzaklo00.bmp');
 
% zaostreni obrazu
h1 = [-0.5 -0.5 -0.5; -0.5 5.0 -0.5; -0.5 -0.5 -0.5];
S1 = imfilter(S0, h1);
imwrite(S1, 'step1.bmp');
 
% preklopeni obrazku kolem svisle osy
S2 = fliplr(S1);
imwrite(S2, 'step2.bmp');
 
% medianovy filtr
S3 = medfilt2(S2, [5 5]);
imwrite(S3, 'step3.bmp');
 
% rozmazani obrazu
h2 = [1 1 1 1 1; 1 3 3 3 1; 1 3 9 3 1; 1 3 3 3 1; 1 1 1 1 1] / 49;
S4 = imfilter(S3, h2);
imwrite(S4, 'step4.bmp');
 
% chyba v obraze
IMaux = fliplr(S0);
IMold = double(IMaux);
IMnew = double(S4);
noise = 0;
for (x = 1:512)
for (y = 1:512)
noise = noise + (abs(IMold(x,y) - IMnew(x,y)));
end;
end;
noise = noise/512/512
 
% roztazeni histogramu
S4 = im2double(S4);
a = min(min(im2double(S4)));
b = max(max(im2double(S4)));
S5 = imadjust(S4, [a b], [0 1]);
S4 = im2uint8(S4);
S5 = im2uint8(S5);
imwrite(S5, 'step5.bmp');
 
% stredni hodnoty a odchylky
S4 = double(S4);
S5 = double(S5);
mean_no_hist = mean2(im2double(S4))
std_no_hist = std2(im2double(S4))
mean_hist = mean2(im2double(S5))
std_hist = std2(im2double(S5))
 
% kvantizace obrazu
N = 2;
l = 0;
m = 255;
p = size(S5);
for (j = 1:p(1))
for (k = 1:p(2))
S6(j,k) = round(((2^N)-1)*(S5(j,k)-l)/(m-l))*(m-l)/((2^N)-1) + l;
end
end
S6 = uint8(S6);
imwrite(S6, 'step6.bmp');
