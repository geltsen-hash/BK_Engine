% Скрипт автоматической компиляции MEX-модуля для MATLAB
% Требуется настроенный C++ компилятор в MATLAB (команда mex -setup C++)

fprintf('=== Компиляция bk_engine_mex.mexw64 ===\n');

try
    mex -O -I. -L. -lBK_Engine bk_engine_mex.cpp -output bk_engine_mex
    fprintf('Компиляция успешно завершена! Создан bk_engine_mex.mexw64\n');
catch ME
    fprintf('Ошибка компиляции MEX: %s\n', ME.message);
    fprintf('Убедитесь, что в MATLAB настроен C++ компилятор (выполните: mex -setup C++)\n');
end
