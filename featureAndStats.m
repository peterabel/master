%% Small Script to plot and check the statistics of the samples
clc; clear(); close all;

%folder
folder = 'birds1';
featureAmount = 72;

for frame=1:1
    
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
    
    indi = zeros(72,1);
    
    for feature = 1:featureAmount
        
        thresh = zeros(72,2);
        indicator = zeros(10,1);
        cnt = 1;
%                 figure;
        for r = feature:featureAmount:(10*featureAmount)
            
            y = linspace(min(min(posSamples(r,:)),min(negSamples(r,:))),max(max(posSamples(r,:)),max(negSamples(r,:))),1000);
            posGauss = exp(-(y-mu_pos(r)).^2./(2*sigma_pos(r)^2))./(sigma_pos(r)*sqrt(2*pi));
            negGauss = exp(-(y-mu_neg(r)).^2./(2*sigma_neg(r)^2))./(sigma_neg(r)*sqrt(2*pi));
            %
%             
%                         subplot(5,2,cnt);
%                         h1 = histogram(posSamples(r,:),'Normalization','pdf');%,'binwidth',500);
%                         title(['Histogram of channel ' num2str(cnt)]);
%                         hold on
%                         plot(y,posGauss,'LineWidth',1.5)
%                         h2 = histogram(negSamples(r,:),'Normalization','pdf','binwidth',h1.get('binwidth'));%,'binwidth',500);
%                         plot(y,negGauss,'LineWidth',1.5)
%                         grid;
%                         legend(['feat' num2str(feature) ' hist'],'gauss approx','bckgrnd hist','bckgrnd approx');
%                         hold off
            
            thresh(r,1) = mu_neg(r)-sigma_neg(r);
            thresh(r,2) = mu_neg(r)+sigma_neg(r);
            indicator(cnt,1) = (mu_pos(r)<=mu_neg(r)-1.6*sigma_neg(r)) || (mu_pos(r)>=mu_neg(r)+1.6*sigma_neg(r) );
            %         disp(['sigma intervall: ' num2str(thresh(r,1)) ' - ' num2str(thresh(r,2))]);
            %         disp(['mean positive  : ' num2str(mu_pos(r))]);
            
            %         k=waitforbuttonpress;
            cnt = cnt+1;
        end
        indi(feature) = sum(indicator);
%                     figure;
%                     imshow(origImg)
%                     x = sampPositions(feature,1);
%                     y = sampPositions(r,2);
%                     w = sampPositions(r,3);
%                     h = sampPositions(r,4);
%                     rectangle('Position',[x,y,w,h],'EdgeColor','r');
    end
    indi = indi/4;
    
    results = zeros(size(origImg,1)+2,size(origImg,2)+2);
    for f=1:featureAmount
        x = sampPositions(f,1)+1;
        y = sampPositions(f,2)+1;
        w = sampPositions(f,3);
        h = sampPositions(f,4);
        
        results(y:(y+h-1),x:(x+w-1)) = results(y:(y+h-1),x:(x+w-1))+indi(f);
    end
    results = results/3;
    figure;
    subplot(1,2,1)
    imshow(results);
    title('Result of the segmentation');
    subplot(1,2,2);
    imshow(origImg);
    title('Frame');
end