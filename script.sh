       while true
       do
        echo  "\n\e[1;34m┌──────────────────────────────────────┐\e[0m"
        echo  "\e[1;34m│\e[0m \e[32mElige una opción: \e[33m                   \e[1;34m│\e[0m"
        echo  "\e[1;34m│\e[0m \e[32m1. Compilar el archivo.\e[0m              \e[1;34m│\e[0m"
        echo  "\e[1;34m│\e[0m \e[32m2. Ejecutar el programa.\e[0m             \e[1;34m│\e[0m"
        echo  "\e[1;34m│\e[0m \e[32m3. Subir la practica a GitHub.\e[0m       \e[1;34m│\e[0m"
        echo  "\e[1;34m│\e[0m \e[32m4. Salir del script.\e[0m                 \e[1;34m│\e[0m"
        echo  "\e[1;34m└──────────────────────────────────────┘\e[0m"

        echo  "\e[32mEscoge una opción: \e[m"
        read select
        case $select in 

            1)
                clear
                gcc MarcosPracticaFinal.c
                ;;
            2)
                clear
                ./a.out
                ;;
            3)
                git pull
                git add MarcosPracticaFinal.c
                git commit -m "Cambios en la practica"
                git push
                ;;
            4)
                clear
                exit
                ;;

          
            *)
                clear
                echo "\e[31mError: Opcion no valida!\e[0m"
                ;;
        esac
    done