CC_FLAGS=-I../../lavm/src -shared -Wall

object/__init__.o: src/__init__.c
	gcc $(CC_FLAGS) -o object/__init__.so src/__init__.c

clean:
	rm object/*