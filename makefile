SRC := ./src
BUILD := ./build
INCLUDE := ./include

# Où copier l'exécutable (~/.local/bin est dans mon $PATH)
BIN_PATH := ~/.local/bin


C_FLAGS := -I $(INCLUDE) -Wall -g -Wuninitialized
C_LIBS := -lncurses -lm

C_FILES := $(wildcard $(SRC)/*.c)
HEADERS := $(wildcard $(INCLUDE)/*.h)
OBJS := $(patsubst $(SRC)/%.c, $(BUILD)/%.o, $(C_FILES))


.PHONY: clean


ram: $(OBJS)
	gcc $(C_FLAGS) $(OBJS) -o $@ $(C_LIBS)
	cp ram $(BIN_PATH)


# Un fichier objet dépend de sa source mais également des headers.
# Ainsi si un header est modifié on recompile tous les objets pour être 
# sûrs que les changements soient bien appliqués partout
$(BUILD)/%.o: $(SRC)/%.c $(HEADERS)
	gcc $(C_FLAGS) -c $< -o $@ $(C_LIBS)


clean:
	rm ./build/* ram
