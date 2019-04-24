using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;

namespace Player
{
    public class Filter
    {
        int width, height;
        byte[] data;
        int[] skip;

        public Filter(byte[] data, int w, int h, int bits)
        {
            this.data = data;
            width = w;
            height = h;
            int c_skip = 1 << bits;
            //формируем массивы для быстрого поиска первой строки шаблона по алгоритму Бойера-Мура-Хорспула
            skip = new int[c_skip];
            for (int i = 0; i < c_skip; i++)
                skip[i] = w;
            for (int i = 0; i < w - 1; i++)
                skip[data[i]] = w - i - 1;
        }

        public bool Match(byte[] img, int w, int h, ref Point pnt)
        {
            if( w < width || h < height) return false;
            int p = 0;
            for (   int i = 0; i < h - height; i++, p += w)
            {
                int j = width - 1;
                int p2 = p + j;
                while (j < w)
                {
                    int z = width - 1;
                    int p3 = p2;
                    do
                    {
                        if (img[p3] != data[z]) break;
                        p3--; z--;
                    } while(z > 0);
                    if (z == 0) //первая строка шаблона совпала
                    {
                        //проверяем на совпадение правый ряд шаблона на совпадение
                        int pp1 = p2 + w;
                        int pp2 = width + width - 1;
                        bool m = true;
                        for (int t = 0; t < height - 1; t++, pp1 += w, pp2 += width)
                        {
                            if (img[pp1] != data[pp2])
                            {
                                m = false;
                                break;
                            }
                        }
                        if (m)
                        {
                            pp1 = p2 + w - width + 1;
                            for (int ii = 1; ii < height; ii++, pp1 += w)
                            {
                                pp2 = pp1;
                                int pp3 = ii * width; 
                                for (int jj = 0; jj < width - 1; jj++, pp2++, pp3++)
                                {
                                    if (img[pp2] != data[pp3])
                                    {
                                        return false;
                                    }
                                }
                            }
                            pnt = new Point(j - width + 1, i);
                            return true;
                        }
                    }
                    int s = skip[img[p2]];
                    j += s;
                    p2 += s;
                }
            }
            return false;
        }
    }
}
