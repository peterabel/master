%% Small Script to plot and check the statistics of the samples
clc; clear(); close all;

%folder
folder = 'basketball';
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
    
    ftr = 1;
    ftr2 = 37; %  Basketball intervall 17 - 56
    
    numBin = 40;
    
    probs = zeros(720,3);
    
    
    chnnl = 1;
    cnt = 1;
    figure;
    for r=1:72:720
    
        surrInv = [r, r+15;r+56, r+71];
        boxInv = [r+16, r+55];
        
        bckgrndFtr = negSamples(r:r+71,1);
        surroundFtr = [posSamples(surrInv(1,1):surrInv(1,2),1)
            posSamples(surrInv(2,1):surrInv(2,2),1)];
        boxFtr = posSamples(boxInv(1):boxInv(2),1);

        binEdges = linspace(min([bckgrndFtr;surroundFtr;boxFtr]),max([bckgrndFtr;surroundFtr;boxFtr]),numBin+1);
        
        subplot(5,2,chnnl);
        h1 = histogram( bckgrndFtr ,'Normalization','probability','BinEdges', binEdges);%'NumBins',numBin);
        title('Histogram in Channel1');
        hold on
        h2 = histogram( surroundFtr ,'Normalization','probability','BinEdges', binEdges);%'NumBins',numBin);
        h3 = histogram(boxFtr,'Normalization','probability','BinEdges', binEdges);%'NumBins',numBin);
        grid;
        legend('background ftrs','groundtruth ftrs');
        title(['Histograms in Channel ' num2str(chnnl)]);
        xlabel('ftr value');
        ylabel('probability');
        legend('background ftr','surround ftr','box ftr');
        hold off  
        
        % Ordering all probabilities        
        for m=r:r+71
            bin = (find(posSamples(m,1) <= binEdges,1));
            
            if bin == numBin + 1
                bin = numBin;
            end
            if bin == 1
                bin = 1;
            end
            
            probs(m,1) = h1.Values(bin);
            probs(m,2) = h2.Values(bin);
            probs(m,3) = h3.Values(bin);
            
           cnt = cnt+1;
        end
        
        chnnl = chnnl+1;

    end
    
        results = zeros(size(origImg,1)+2,size(origImg,2)+2,10);
        
        
chnnl = 1;
for r=1:72:720
    
    
    figure;
    
    for f=r:(r+71)
        x = sampPositions(f,1)+1;
        y = sampPositions(f,2)+1;
        w = sampPositions(f,3);
        h = sampPositions(f,4);
        results(y:(y+h-1),x:(x+w-1),chnnl) = results(y:(y+h-1),x:(x+w-1),chnnl)+probs(f,1);        
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