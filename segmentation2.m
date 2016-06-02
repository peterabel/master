%% Small Script to plot and check the statistics of the samples
clc; clear(); close all hidden;

%folder
folder = 'birds1';
featureAmount = 72;

for frame=1:2
    
    if isunix == 1
        negSamples = importdata([folder '/frame' num2str(frame) '/negSamples.txt']);
        posSamples = importdata([folder '/frame' num2str(frame)  '/posSamples.txt']);
        negMuSigSq = importdata([folder '/frame' num2str(frame) '/negMuSigSq.txt']);
        posMuSigSq = importdata([folder '/frame' num2str(frame)  '/posMuSigSq.txt']);
        origImg = imread([folder '/groundTruth' num2str(frame)  '.png']);
        if frame==1
            sampPositions = importdata([folder '/frame' num2str(frame) '/samplePositions.txt']);
        end
    else
        negSamples = importdata([folder '\frame' num2str(frame) '\negSamples.txt']);
        posSamples = importdata([folder '\frame' num2str(frame)  '\posSamples.txt']);
        negMuSigSq = importdata([folder '\frame' num2str(frame) '\negMuSigSq.txt']);
        posMuSigSq = importdata([folder '\frame' num2str(frame)  '\posMuSigSq.txt']);
        origImg = imread([folder '\groundTruth' num2str(frame)  '.png']);
        sampPositions = importdata([folder '\frame' num2str(frame) '\samplePositions.txt']);        
    end
    
    %Basketball intervall 17 - 56
    
    numBin = 40;
    
    indeces = zeros(720,1);
    probs = zeros(720,2);
    probs2 = zeros(720,2);
    D = zeros(10,1);%numBin*(numBin-1)*0.5);
    stats = zeros(10,4);
    
    chnnl = 1;
    cnt = 1;
    fg1 = figure;
    samp = 1;
    for r=1:72:720
        
        %for basketball
        if strcmp(folder, 'basketball')
            surrInv = [r, r+15;r+56, r+71];
            boxInv = [r+16, r+55];
            surroundFtr = [posSamples(surrInv(1,1):surrInv(1,2),samp)
                posSamples(surrInv(2,1):surrInv(2,2),samp)];
            boxFtr = posSamples(boxInv(1):boxInv(2),samp);
        end
        %for birds1
        if strcmp(folder,'birds1')
            surrInv = r+[1:3,7:11,15:19,23:27,31:35,39:43,47:51,55:59,63:71]-1;
            boxInv = r+[4,5,6,12,13,14,20,21,22,28,29,30,36,37,38,44,45,46,52,53,54,60,61,62]-1;
            surroundFtr = posSamples(surrInv,samp);
            boxFtr = posSamples(boxInv,samp);
        end
        
        % Estimating the probability density
        [f_1, xi_1] = ksdensity(boxFtr,'Kernel','epanechnikov');
        [f_0, xi_0] = ksdensity(surroundFtr,'Kernel','epanechnikov');
        
        % Gauss Approximation for decision
        mu_1 = mean(boxFtr);
        sigma_1 = sqrt(var(boxFtr));
        mu_0 = mean( surroundFtr );
        sigma_0 = sqrt(var( surroundFtr ));
        
        stats(chnnl,1) = mu_0;
        stats(chnnl,2) = sigma_0;
        stats(chnnl,3) = mu_1;
        stats(chnnl,4) = sigma_1;
        stats(chnnl,5) = median(surroundFtr);
        stats(chnnl,6) = median(boxFtr);
        
        y = linspace(min([surroundFtr; boxFtr]),max([surroundFtr; boxFtr]),1000);
        y_steps = (max([surroundFtr; boxFtr])-min([surroundFtr; boxFtr]))/1000;
        posGauss = exp(-(y-mu_1).^2./(2*sigma_1^2))./(sigma_1*sqrt(2*pi));
        negGauss = exp(-(y-mu_0).^2./(2*sigma_0^2))./(sigma_0*sqrt(2*pi)); 
        
        binEdges = linspace(min([surroundFtr;boxFtr]),max([surroundFtr;boxFtr]),numBin+1);
        
        figure(fg1);
        subplot(5,2,chnnl);
        h1 = histogram( surroundFtr ,'Normalization','pdf','BinEdges', binEdges);%'NumBins',numBin);
        title('Histogram in Channel1');
        hold on
        h2 = histogram(boxFtr,'Normalization','pdf','BinEdges', binEdges);%'NumBins',numBin);
%         plot(y,negGauss,'LineWidth',2);
%         plot(y,posGauss,'LineWidth',2);
        plot(xi_0,f_0,'LineWidth',2);
        plot(xi_1,f_1,'LineWidth',2);
        grid;
        title(['Histograms in Channel ' num2str(chnnl)]);
        xlabel('ftr value');
        ylabel('pdf');
        legend('surround ftr','box ftr','f0','f1');
        
%         D(chnnl,:) = pdist([h2.Values', h1.Values'],'chebychev');
        cheby = pdist([posGauss', negGauss'],'chebychev');
        D(chnnl) = sum(cheby);
        
        % Ordering all probabilities
        for m=r:(r+71)
            bin = (find(posSamples(m,samp) <= binEdges,1));         
            if bin == numBin + 1
                bin = numBin;
            end
            if bin == 1
                bin = 1;
            end
            
%             probs(m,1) = h1.Values(bin);
%             probs(m,2) = h2.Values(bin);
            
            %find index
            factors = 0.5:0.25:2;
            cnt = 1;
            if posSamples(m,samp) < xi_1(1)
                index = 2;
            elseif posSamples(m,samp) > xi_1(end)
                index = length(xi_1);
            else
                index = find((abs(xi_1-posSamples(m,samp))<0.25*(xi_1(2)-xi_1(1)))== 1);
            end
            
            while isempty(index)
                index = find((abs(xi_1-posSamples(m,samp))<factors(cnt)*(xi_1(2)-xi_1(1)))== 1);
                cnt = cnt+1;
            end
%             if size(index,2)>1
%                 index = index(1);
%             end
            
            probs(m,1) = f_0(index);
            probs(m,2) = f_1(index);
            
%             probs2(m,1) = trapz(y(1:index),negGauss(1:index));
%             probs2(m,2) = trapz(y(1:index),posGauss(1:index));
            
        end
        
        
        chnnl = chnnl+1;
        
    end
    
    results = zeros(size(origImg,1)+1,size(origImg,2)+1,10);   
    resNeg = zeros(size(origImg,1)+1,size(origImg,2)+1,10);
    resPos = zeros(size(origImg,1)+1,size(origImg,2)+1,10);
    chnnl = 1;
    for r=1:72:720
        for f=r:(r+71)
            x = sampPositions(f,1)+1;
            y = sampPositions(f,2)+1;
            w = sampPositions(f,3);
            h = sampPositions(f,4);
%             if probs(f,2) >= probs(f,1)
                resPos(y:(y+h-2),x:(x+w-2),chnnl) = resPos(y:(y+h-2),x:(x+w-2),chnnl)+probs(f,2);
%             end
            if probs(f,2) < probs(f,1)
                resNeg(y:(y+h-1),x:(x+w-1),chnnl) = resNeg(y:(y+h-1),x:(x+w-1),chnnl)+probs(f,1);
            end
        end
        resNeg(:,:,chnnl) = resNeg(:,:,chnnl)/4;
        resPos(:,:,chnnl) = resPos(:,:,chnnl)/4;
        results(:,:,chnnl) = resPos(:,:,chnnl)-resNeg(:,:,chnnl);
        chnnl = chnnl+1;
    end
    
    
%     figure;
    score = zeros(10,1);
    for j=1:10
%         subplot(5,2,j);
%         colormap('hot');
%         imagesc(resPos(:,:,j));
        score(j) = abs(stats(j,5)-stats(j,6))*(1/stats(j,4));
    end
    [val, in] = sort(score,'descend');
    
    chebyDist = sum(D,2);
    [valCheby, inCheby] = sort(chebyDist,'ascend');
%     normWeight = sum(valCheby(1:10));
%     weights = (valCheby)/normWeight;
    normWeight = sum(1./valCheby(1:3));
    weights = (1./valCheby)/normWeight;

    
    
    figure

%     finImg = sum(resPos(:,:,in(1:3)),3)/3;
    finImg = sum(resPos(:,:,inCheby(1:3)),3)/3;

%     finImg = weights(1).*resPos(:,:,inCheby(1)) + ...
%         weights(2).*resPos(:,:,inCheby(2)) + ...
%         weights(3).*resPos(:,:,inCheby(3)) + ...
%         weights(4).*resPos(:,:,inCheby(4)) + ...
%         weights(5).*resPos(:,:,inCheby(5)) + ...
%         weights(6).*resPos(:,:,inCheby(6)) + ...
%         weights(7).*resPos(:,:,inCheby(7)) + ...
%         weights(8).*resPos(:,:,inCheby(8)) + ...
%         weights(9).*resPos(:,:,inCheby(9)) + ...
%         weights(10).*resPos(:,:,inCheby(10));
    colormap('hot')
    imagesc(finImg)
    colorbar
    figure;
    imshow(origImg);

    
    
%     figure;
%     imshow(origImg)
%     x = sampPositions(ftr,1);
%     y = sampPositions(ftr,2);
%     w = sampPositions(ftr,3);
%     h = sampPositions(ftr,4);
%     rectangle('Position',[x,y,w,h],'EdgeColor','r');
%     x = sampPositions(ftr2,1);
%     y = sampPositions(ftr2,2);
%     w = sampPositions(ftr2,3);
%     h = sampPositions(ftr2,4);
%     rectangle('Position',[x,y,w,h],'EdgeColor','b');

end