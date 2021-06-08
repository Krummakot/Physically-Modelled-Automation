
#ifndef AutoString_hpp
#define AutoString_hpp

#include <JuceHeader.h>

//using namespace std;

class AutoString : public juce::Component {
    
public:
    
    // Constructor function (special function - no return type, name = Class name)
    AutoString();
    AutoString(NamedValueSet& parameters, double k);
    
    // Destructor
    ~AutoString();

    
    float processSample(float l, float outPos);
    void processScheme();
    void updateStates();
    
//    void paint (juce::Graphics& g) override;
//    void resized() override;
//       
//    Path visualiseState (Graphics& g, double visualScaling); // function to draw the state of the string
    
    void setPluckLoc(double pluckLoc);
    void setOutLoc(double outLoc);
    
    void setDamping(double s0, double s1);
    void setE(double newE);
    void setTension(double newTension);
    void setStringLength(double newLength);
    
    void setN(double N);
    void setH(double h);
    void setLambdaSq(double newLambdaSq);
    void setMuSq(double newMuSq);
    
    void updateCoefficients();
    
    void setNodes(int startNode, int endNode);
    
    
    void excite(int excitationSelection, double velocity, int newWidth);
    void exciteHann();
    void exciteTri();
    
    double getOutput(double outPos);
    
    std::vector<std::vector<double>> uStates;
    std::vector<double*> uPtr;
private:
    
    
    double fs, f0, c, k, s0, s1, h, N, lambdaSq, muSq, pluckLoc, outPos, stringPluckRatio, kappa, width;
    
    double damp, stiffness, tension, stringDiameter, stringRadius, p, A, E, I;
    int stringLength;
    
    double startNode = 2;
    double endNode;
    
    double A1, A2, A3, A4, A5, B1, B2, D;
    
    bool clamped = true;
    
};



#endif /* AutoString_hpp */
