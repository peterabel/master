%% Small Script to plot and check the statistics of the samples
clc; clear(); close all;

%folder
folder = 'birds1';
featureAmount = 72;

for frame=1
    
    %Loading all Samples of all channels
    negSamples = importdata([folder '\frame' num2str(frame) '\negSamples.txt']);
    posSamples = importdata([folder '\frame' num2str(frame)  '\posSamples.txt']);
    negMuSigSq = importdata([folder '\frame' num2str(frame) '\negMuSigSq.txt']);
    posMuSigSq = importdata([folder '\frame' num2str(frame)  '\posMuSigSq.txt']);
    origImg = imread([folder '\groundTruth' num2str(frame)  '.png']);
    sampPositions = importdata([folder '\frame' num2str(frame) '\samplePositions.txt']);
    
    
    %Positive Mean and variance
    meanValuesPositive = posMuSigSq(:,1);
    varValuesPositive = posMuSigSq(:,2);
    meanValuesNegative = negMuSigSq(:,1);
    varValuesNegative = negMuSigSq(:,2);
    
    %Negative mean and variance
    mu_pos = meanValuesPositive;
    sigma_pos = sqrt(varValuesPositive);
    mu_neg = meanValuesNegative;
    sigma_neg = sqrt(varValuesNegative);
    
    
    res = zeros(72,2);
    gauss_all_pos = zeros(72,1000);
    X = zeros(72,1000);     
    binAmount = 5;
    histValues = zeros(720,binAmount); 
    histEdges = zeros(720,binAmount+1);
    prob = zeros(720,1);
    
    gauss_all_neg = zeros(72,1000);
    
    for feature = 1:720%
        
        [hValTemp, hEdgTemp] = histcounts(posSamples(feature,:),'Normalization','probability','NumBins',binAmount);
        histValues(feature,:) = hValTemp;
        histEdges(feature,:) = hEdgTemp;
        bin = (find(posSamples(feature,1)<=hEdgTemp,1));
        if bin == binAmount + 1
            bin = binAmount;
        end
        if bin == 1
            bin = 1;
        end
        prob(feature) = hValTemp(bin);

        % Gauss Approximation for decision
        y = linspace(min(negSamples(feature,:)),max(posSamples(feature,:)),1000);
        posGauss = exp(-(y-mu_pos(feature)).^2./(2*sigma_pos(feature)^2))./(sigma_pos(feature)*sqrt(2*pi));
        negGauss = exp(-(y-mu_neg(feature)).^2./(2*sigma_neg(feature)^2))./(sigma_neg(feature)*sqrt(2*pi));
        
        gauss_all_pos(feature,:) = posGauss;
        gauss_all_neg(feature,:) = negGauss;
        X(feature,:) = y;
        
        % Creating a measure indicating the distance between sample and
        % gauss approximation
        res(feature,1) = abs(posSamples(feature,1)-posMuSigSq(feature,1))/sqrt(posMuSigSq(feature,2));
        res(feature,2) = abs(posSamples(feature,1)-negMuSigSq(feature,1))/sqrt(negMuSigSq(feature,2));
        
%         figure;
%         h1 = histogram(posSamples(feature,:),'Normalization','pdf');%,'binwidth',500);
%         title(['Histogram of channel ' num2str(feature)]);
%         hold on
%         h2 = histogram(negSamples(feature,:),'Normalization','pdf','binwidth',h1.get('binwidth'));%,'binwidth',500);
%         plot(X(feature,:),gauss_all_pos(feature,:),'LineWidth',1.5)       
%         plot(X(feature,:),gauss_all_neg(feature,:),'LineWidth',1.5)
%         plot(posSamples(feature,1),0,'x','LineWidth',1.5)
%         grid;
%         legend(['feat' num2str(feature) ' hist'],'gauss approx','bckgrnd hist','bckgrnd approx');
%         hold off            
    end
    [value, decision] = min(res,[],2);
%     indi = indi/4;
    
%     results = zeros(size(origImg,1)+2,size(origImg,2)+2);
%     for f=1:featureAmount
%         x = sampPositions(f,1)+1;
%         y = sampPositions(f,2)+1;
%         w = sampPositions(f,3);
%         h = sampPositions(f,4);
%
%         results(y:(y+h-1),x:(x+w-1)) = results(y:(y+h-1),x:(x+w-1))+indi(f);
%     end
%     results = results/3;
%     figure;
%     subplot(1,2,1)
%     imshow(results);
%     title('Result of the segmentation');
%     subplot(1,2,2);
%     imshow(origImg);
%     title('Frame');

    results = zeros(size(origImg,1)+2,size(origImg,2)+2,10);
    chnnl = 1;
for r=1:72:720
    
    
    figure;
    
    for f=r:(r+71)
        x = sampPositions(f,1)+1;
        y = sampPositions(f,2)+1;
        w = sampPositions(f,3);
        h = sampPositions(f,4);
        if decision(f) == 1
            results(y:(y+h-1),x:(x+w-1),chnnl) = results(y:(y+h-1),x:(x+w-1),chnnl)+prob(f);
        end        
    title(['Channel ' num2str(floor(r/72))+1]);
    
    %--------------------------- Drawing Boxes ---------------------------%
%     
%     figure;
%     imshow(origImg)
%     title(['Channel ' num2str(floor(chnnl/72))+1]);
%     for f=chnnl:(chnnl+71)
%         x = sampPositions(f,1);
%         y = sampPositions(f,2);
%         w = sampPositions(f,3);
%         h = sampPositions(f,4);
%         if decision(f) == 1
%             rectangle('Position',[x,y,w,h],'EdgeColor','r');
%         end
%         if decision(f) == 2
%             rectangle('Position',[x,y,w,h],'EdgeColor','b');
%         end
    %---------------------------------------------------------------------%
    end
    results(:,:,chnnl) = results(:,:,chnnl);
    imshow(results(:,:,chnnl));
    chnnl = chnnl+1;
end
figure;
finImg = sum(results,3)/10;
imshow(finImg);
figure;
imshow(origImg);
end