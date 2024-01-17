#include <cmath>

int circle(int index){
    int size = 240;
    if (index >= size) {
        return index - size;
    }
    if (index < 0) {
        return index + size;
    }
    return index;
}

double max(double a, double b){
    if (a >= b){
        return a;
    } else {
        return b;
    }
}

double min(double a, double b){
    if (a <= b){
        return a;
    } else{
        return b;
    }
}

void rgb_to_hsv(int *RGB, double *hsv){

    double R_p = RGB[0]/255.0;
    double G_p = RGB[1]/255.0;
    double B_p = RGB[2]/255.0;
    double max_r_g = max(R_p, G_p);
    double C_max = max(max_r_g, B_p);
    double min_r_g = min(R_p, G_p);
    double C_min = min(min_r_g, B_p);
    double delta = C_max - C_min;
    double H;
    if (delta == 0.0) {
        H = 0.0;
    } else if (C_max == R_p){
        H = int(60 * ((G_p - B_p) / delta) + 360) % 360;
    } else if (C_max == G_p){
        H = int(60 * ((B_p - R_p)/delta) + 120) % 360;
    } else if (C_max == B_p){
        H = int(60 * ((R_p - G_p)/delta) + 240) % 360;
    }

    double S;
    if (C_max == 0.0){
        S = 0.0;
    } else {
        S = delta/C_max;
    }
    hsv[0] = H;
    hsv[1] = S;
    hsv[2] = C_max;
}

int abs(int x){
    if (x < 0){
        return x*-1;
    }
    return x;
}

void hsv_to_rgb(double *HSV, int *RGB){
    double H = HSV[0];
    double S = HSV[1];
    double V = HSV[2];
    double C = V*S;
    double X = C * (1- abs(std::fmod(H/60, 2.0) - 1));
    double m = V - C;
    double R;
    double G;
    double B;
    if (H < 60){
        R = C;
        G = X;
        B = 0;
    } else if (H < 120){
        R = X;
        G = C;
        B = 0;
    } else if (H < 180){
        R = 0;
        G = C;
        B = X;
    } else if (H < 240){
        R = 0;
        G = X;
        B = C;
    } else if (H < 300){
        R = X;
        G = 0;
        B = C;
    } else {
        R = C;
        G = 0;
        B = X;
    }
    RGB[0] = (R+m)*255.0;
    RGB[1] = (G+m)*255.0;
    RGB[2] = (B+m)*255.0;
}



void hot_seat2(AddressableLight &it, int target_start, int target_end, const Color &current_color) {
    int current_start = NULL;
    int current_end = NULL;
    bool searching_for_start = true;
    bool searching_for_end = true;
    bool check_tail = false;

    // Figure out which is the current start and end pixels
    bool found_start = false;
    bool found_end = false;
    for (int i=0; i<it.size(); i++) {
        if (it[i].get().is_on()) {
            // look left
            int j = i;
            while (true){
                if (it[circle(j-1)].get().is_on()){
                    j--;
                    if (circle(j) == i){
                        // we went all the way around and didn't find it
                        break;
                    }
                } else {
                    current_start = circle(j);
                    found_start = true;
                    break;
                }
            }
            // look right
            int k = i;
            while (true) {
                if(it[circle(k+1)].get().is_on()){
                    k++;
                    if (circle(k) == i){
                        // we went all the way around and didn't find it
                        break;
                    }
                } else {
                    current_end = circle(k);
                    found_end = true;
                    break;
                }
            }
        }
    }
    if (!found_start || !found_end) {
        it.all().fade_to_black(100);
        it.range(target_start, target_end + 1) = current_color;
        return;
    }
    
    bool direction;
    float start_distance = sin((target_start - current_start) * (2*3.1415)/it.size());
    float end_distance = sin((target_end - current_end) * (2*3.1415)/it.size());
    if (start_distance == 0) {
        direction = end_distance > 0;
    } else {
        direction = start_distance > 0;
    }
    
    Color mcc = it[current_start].get();
    double mcc_hsv[3];
    int mcc_rgb[3] = {mcc.r, mcc.g, mcc.b};
    rgb_to_hsv(mcc_rgb, mcc_hsv);
    double target_hsv[3];
    int target_rgb[3] = {current_color.r, current_color.g, current_color.b};
    rgb_to_hsv(target_rgb, target_hsv);

    int start_steps = abs(target_start - current_start);
    if (start_steps > it.size()/2) {
        start_steps = it.size() - start_steps;
    }
    int end_steps = abs(target_end - current_end);
    if (end_steps > it.size()/2) {
        end_steps = it.size() - end_steps;
    }
    int steps;
    if (start_steps >= end_steps) {
        steps = start_steps;
    } else {
        steps = end_steps;
    }

    double new_HSV[3];
    if (steps > 0) {
        new_HSV[0] = mcc_hsv[0] + (target_hsv[0] - mcc_hsv[0])/steps;
        new_HSV[1] = mcc_hsv[1] + (target_hsv[1] - mcc_hsv[1])/steps;
        new_HSV[2] = mcc_hsv[2] + (target_hsv[2] - mcc_hsv[2])/steps;
    } else {
        new_HSV[0] = target_hsv[0];
        new_HSV[1] = target_hsv[1];
        new_HSV[2] = target_hsv[2];
    }

    int new_RGB[3];
    hsv_to_rgb(new_HSV, new_RGB);
    Color new_color = Color(new_RGB[0], new_RGB[1], new_RGB[2]);

    if (current_start > current_end) {
        it.range(current_start, it.size()) = new_color;
        it.range(0, current_end+1) = new_color;
    } else {
        it.range(current_start, current_end + 1) = new_color;
    }
    
    if ((current_start != target_start || current_end != target_end) && direction) {
        // we are shifting counter-clockwise
        if (current_start > target_start){
            current_start -= it.size();
        }
        if (current_end > target_end) {
            current_end -= it.size();
        }
        int end;
        if (current_start != target_start){
            end = current_start;
        } else {
            end = target_start + 1;
        }
        for (int i=target_end; i>=end; i--){
            if (current_start == target_start && i == current_start){
                break;
            }
            it[circle(i)] = it[circle(i-1)].get();
        }
    } else if ((current_end != target_end || current_start != target_start) && !direction) {
        // shifting clockwise
        if (current_end < target_end){
            current_end += it.size();
        }
        if (current_start < target_start) {
            current_start += it.size();
        }
        int end;
        if (current_end != target_end){
            end = current_end;
        } else {
            end = target_end;
        }
        for (int i=target_start; i<=end; i++){
            if (current_end == target_end && i == current_end){
                break;
            }
            it[circle(i)] = it[circle(i+1)].get();
        }
    }
}

void hot_seat(AddressableLight &it, int target_start, int target_end, const Color &current_color, int speed) {
    for (int i=0; i < speed; i++){
        hot_seat2(it, target_start, target_end, current_color);
    }
}