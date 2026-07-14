#version 330 core
in vec2 v_LocalPos;
in float v_Type;
in vec4 v_Color;
out vec4 FragColor;

void main() {
    float dist = length(v_LocalPos);
    
    bool shouldDiscard = false;

    if (v_Type < 0.5) { //Basic cell
        if (dist > 1.0) shouldDiscard = true;
    } 
    else if (v_Type < 1.5) { //Another cell
        shouldDiscard = false; 
    }
    else {

    }

    if (shouldDiscard) {
        discard;
    }
    
    FragColor = v_Color;
}